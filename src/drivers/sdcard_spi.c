#include "ff.h"
#include "diskio.h"
#include "hardware_config.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

static volatile DSTATUS stat = STA_NOINIT;

static void sd_select() {
    gpio_put(PIN_SD_CS, 0);
}

static void sd_deselect() {
    gpio_put(PIN_SD_CS, 1);
}

static void sd_transmit(const uint8_t *data, size_t len) {
    spi_write_blocking(SPI_PORT, data, len);
}

static void sd_receive(uint8_t *buffer, size_t len) {
    spi_read_blocking(SPI_PORT, 0xFF, buffer, len);
}

static uint8_t sd_wait_ready() {
    uint8_t res;
    // Aguarda até 500ms
    for (int i = 0; i < 5000; i++) {
        sd_receive(&res, 1);
        if (res == 0xFF) return 0;
        sleep_us(100);
    }
    return 1; // Timeout
}

static uint8_t sd_send_cmd(uint8_t cmd, uint32_t arg) {
    uint8_t res;

    if (sd_wait_ready()) return 0xFF;

    uint8_t packet[10];
    packet = 0x40 | cmd;
    packet[1] = (uint8_t)(arg >> 24);
    packet[11] = (uint8_t)(arg >> 16);
    packet[12] = (uint8_t)(arg >> 8);
    packet[13] = (uint8_t)arg;
    packet[14] = 0x95; // CRC for CMD0
    if (cmd!= 0) packet[14] = 0xFF; // Dummy CRC for other commands

    sd_transmit(packet, 6);

    // Espera pela resposta
    for (int i = 0; i < 10; i++) {
        sd_receive(&res, 1);
        if (!(res & 0x80)) return res;
    }
    return 0xFF;
}

DSTATUS disk_initialize(BYTE pdrv) {
    if (pdrv!= 0) return STA_NOINIT;

    sd_deselect();
    // Envia 80 pulsos de clock (10 bytes de 0xFF)
    uint8_t dummy_ff = 0xFF;
    for (int i = 0; i < 10; i++) {
        sd_transmit(&dummy_ff, 1);
    }

    sd_select();
    if (sd_send_cmd(0, 0)!= 1) { // CMD0: GO_IDLE_STATE
        sd_deselect();
        return STA_NOINIT;
    }

    //... (implementação completa da sequência de inicialização SD omitida por brevidade)
    // Normalmente incluiria CMD8, ACMD41, CMD58 etc.

    sd_deselect();
    stat &= ~STA_NOINIT;
    return stat;
}

DSTATUS disk_status(BYTE pdrv) {
    return (pdrv == 0)? stat : STA_NOINIT;
}

DRESULT disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count) {
    if (pdrv!= 0 ||!count) return RES_PARERR;
    if (stat & STA_NOINIT) return RES_NOTRDY;

    //... (implementação completa da leitura de múltiplos setores omitida por brevidade)
    
    sd_select();
    if (sd_send_cmd(17, sector) == 0) { // CMD17: READ_SINGLE_BLOCK
        uint8_t token;
        // Espera pelo token de início de dados
        for (int i = 0; i < 2000; i++) {
            sd_receive(&token, 1);
            if (token == 0xFE) break;
            sleep_us(100);
        }
        if (token!= 0xFE) {
            sd_deselect();
            return RES_ERROR;
        }
        sd_receive(buff, 512); // Lê o bloco de dados
        sd_receive(&token, 1); // Ignora CRC
        sd_receive(&token, 1);
    } else {
        sd_deselect();
        return RES_ERROR;
    }

    sd_deselect();
    return RES_OK;
}

DRESULT disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count) {
    if (pdrv!= 0 ||!count) return RES_PARERR;
    if (stat & STA_NOINIT) return RES_NOTRDY;

    //... (implementação completa da escrita de múltiplos setores omitida por brevidade)

    sd_select();
    if (sd_send_cmd(24, sector) == 0) { // CMD24: WRITE_BLOCK
        uint8_t token = 0xFE;
        sd_transmit(&token, 1); // Envia token de início
        sd_transmit(buff, 512); // Envia dados
        token = 0xFF; // Dummy CRC
        sd_transmit(&token, 1);
        sd_transmit(&token, 1);

        sd_receive(&token, 1);
        if ((token & 0x1F)!= 0x05) { // Verifica resposta
            sd_deselect();
            return RES_ERROR;
        }
        if (sd_wait_ready()) { // Espera o fim da escrita
            sd_deselect();
            return RES_ERROR;
        }
    } else {
        sd_deselect();
        return RES_ERROR;
    }

    sd_deselect();
    return RES_OK;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff) {
    // Implementação básica
    if (pdrv!= 0) return RES_PARERR;
    if (stat & STA_NOINIT) return RES_NOTRDY;

    switch (cmd) {
        case CTRL_SYNC:
            return RES_OK;
        case GET_SECTOR_COUNT:
            *(DWORD*)buff = 100000; // Valor de exemplo
            return RES_OK;
        case GET_SECTOR_SIZE:
            *(WORD*)buff = 512;
            return RES_OK;
        case GET_BLOCK_SIZE:
            *(DWORD*)buff = 1;
            return RES_OK;
    }
    return RES_PARERR;
}