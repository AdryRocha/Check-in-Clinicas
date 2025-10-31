// src/drivers/sdcard_spi.c
#include "ff.h"
#include "diskio.h"
#include "include/hardware_config.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

static volatile DSTATUS stat = STA_NOINIT;

static void sd_select(void) {
    gpio_put(PIN_SD_CS, 0);
    sleep_us(1);
}

static void sd_deselect(void) {
    gpio_put(PIN_SD_CS, 1);
    sleep_us(1);
}

static void sd_transmit(const uint8_t *data, size_t len) {
    spi_write_blocking(SPI_PORT, data, len);
}

static void sd_receive(uint8_t *buffer, size_t len) {
    spi_read_blocking(SPI_PORT, 0xFF, buffer, len);
}

static uint8_t sd_wait_ready(void) {
    uint8_t res;
    for (int i = 0; i < 5000; i++) {
        sd_receive(&res, 1);
        if (res == 0xFF) return 0;
        sleep_us(100);
    }
    return 1;
}

static uint8_t sd_send_cmd(uint8_t cmd, uint32_t arg) {
    uint8_t res;

    if (sd_wait_ready()) return 0xFF;

    uint8_t packet[6];
    packet[0] = 0x40 | cmd;
    packet[1] = (uint8_t)(arg >> 24);
    packet[2] = (uint8_t)(arg >> 16);
    packet[3] = (uint8_t)(arg >> 8);
    packet[4] = (uint8_t)arg;
    
    if (cmd == 0) {
        packet[5] = 0x95;
    } else if (cmd == 8) {
        packet[5] = 0x87;
    } else {
        packet[5] = 0xFF;
    }

    sd_transmit(packet, 6);

    for (int i = 0; i < 10; i++) {
        sd_receive(&res, 1);
        if (!(res & 0x80)) return res;
    }
    return 0xFF;
}

DSTATUS disk_initialize(BYTE pdrv) {
    if (pdrv != 0) return STA_NOINIT;

    sd_deselect();
    
    uint8_t dummy[10];
    for (int i = 0; i < 10; i++) {
        dummy[i] = 0xFF;
    }
    sd_transmit(dummy, 10);

    sd_select();
    
    if (sd_send_cmd(0, 0) != 1) {
        sd_deselect();
        return STA_NOINIT;
    }

    uint8_t cmd8_response[4];
    if (sd_send_cmd(8, 0x1AA) == 1) {
        sd_receive(cmd8_response, 4);
        
        if (cmd8_response[2] == 0x01 && cmd8_response[3] == 0xAA) {
            uint16_t timeout = 1000;
            do {
                sd_send_cmd(55, 0);
                if (sd_send_cmd(41, 0x40000000) == 0) break;
                sleep_ms(1);
            } while (--timeout);
            
            if (timeout == 0) {
                sd_deselect();
                return STA_NOINIT;
            }
            
            if (sd_send_cmd(58, 0) == 0) {
                uint8_t ocr[4];
                 sd_receive(ocr, 4);
            }
        }
    }

    sd_deselect();
    stat &= ~STA_NOINIT;
    return stat;
}

DSTATUS disk_status(BYTE pdrv) {
    return (pdrv == 0) ? stat : STA_NOINIT;
}

DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count) {
    if (pdrv != 0 || !count) return RES_PARERR;
    if (stat & STA_NOINIT) return RES_NOTRDY;

    for (UINT i = 0; i < count; i++) {
        sd_select();
        
        if (sd_send_cmd(17, sector + i) == 0) {
            uint8_t token;
            uint16_t timeout = 2000;
            
            do {
                sd_receive(&token, 1);
                if (token == 0xFE) break;
                sleep_us(100);
            } while (--timeout);
            
            if (token != 0xFE) {
                sd_deselect();
                return RES_ERROR;
            }
            
            sd_receive(buff + (i * 512), 512);
            
            uint8_t crc[2];
            sd_receive(crc, 2);
        } else {
            sd_deselect();
            return RES_ERROR;
        }
        
        sd_deselect();
    }

    return RES_OK;
}

DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count) {
    if (pdrv != 0 || !count) return RES_PARERR;
    if (stat & STA_NOINIT) return RES_NOTRDY;
    if (stat & STA_PROTECT) return RES_WRPRT;

    for (UINT i = 0; i < count; i++) {
        sd_select();
        
        if (sd_send_cmd(24, sector + i) == 0) {
            uint8_t token = 0xFE;
            sd_transmit(&token, 1);
            
            sd_transmit(buff + (i * 512), 512);
            
            uint8_t crc[2] = {0xFF, 0xFF};
            sd_transmit(crc, 2);
            
            uint8_t response;
            sd_receive(&response, 1);
            
            if ((response & 0x1F) != 0x05) {
                sd_deselect();
                return RES_ERROR;
            }
            
            if (sd_wait_ready()) {
                sd_deselect();
                return RES_ERROR;
            }
        } else {
            sd_deselect();
            return RES_ERROR;
        }
        
        sd_deselect();
    }

    return RES_OK;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff) {
    if (pdrv != 0) return RES_PARERR;
    if (stat & STA_NOINIT) return RES_NOTRDY;

    DRESULT res = RES_ERROR;

    switch (cmd) {
        case CTRL_SYNC:
            sd_select();
            if (sd_wait_ready() == 0) {
                res = RES_OK;
            }
            sd_deselect();
            break;

        case GET_SECTOR_COUNT:
            *(DWORD*)buff = 1000000;
            res = RES_OK;
            break;

        case GET_SECTOR_SIZE:
            *(WORD*)buff = 512;
            res = RES_OK;
            break;

        case GET_BLOCK_SIZE:
            *(DWORD*)buff = 1;
            res = RES_OK;
            break;

        default:
            res = RES_PARERR;
    }

    return res;
}