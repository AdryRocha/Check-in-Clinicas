#include "r307s_fingerprint.h"
#include "hardware_config.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <string.h>

static const uint8_t R307S_START_CODE = {0xEF, 0x01};
static uint32_t r307s_password = 0;
static uint32_t r307s_address = 0xFFFFFFFF;

static void write_packet(uint8_t type, const uint8_t *packet, uint16_t len) {
    uint16_t sum = 0;
    uart_write_blocking(UART_FP_ID, R307S_START_CODE, 2);
    uart_write_blocking(UART_FP_ID, (uint8_t *)&r307s_address, 4);
    uart_write_blocking(UART_FP_ID, &type, 1);
    
    uint16_t length = len + 2;
    uint8_t len_high = (length >> 8) & 0xFF;
    uint8_t len_low = length & 0xFF;
    
    uart_write_blocking(UART_FP_ID, &len_high, 1);
    uart_write_blocking(UART_FP_ID, &len_low, 1);
    
    sum = type + len_high + len_low;
    
    for (uint16_t i = 0; i < len; i++) {
        uart_write_blocking(UART_FP_ID, &packet[i], 1);
        sum += packet[i];
    }
    
    uint8_t sum_high = (sum >> 8) & 0xFF;
    uint8_t sum_low = sum & 0xFF;
    
    uart_write_blocking(UART_FP_ID, &sum_high, 1);
    uart_write_blocking(UART_FP_ID, &sum_low, 1);
}

static int read_packet(uint8_t *buffer, uint16_t len, uint16_t timeout) {
    int bytes_read = 0;
    uint64_t start_time = time_us_64();
    
    while ((time_us_64() - start_time) < (timeout * 1000)) {
        if (uart_is_readable(UART_FP_ID) && bytes_read < len) {
            buffer[bytes_read++] = uart_getc(UART_FP_ID);
        }
        if (bytes_read >= 9) { // Minimum packet size
            if (buffer!= 0xEF |

| buffer![1]= 0x01) {
                // Invalid header, reset
                bytes_read = 0;
                continue;
            }
            uint16_t packet_len = (buffer[2] << 8) | buffer[3];
            if (bytes_read >= (packet_len + 9)) {
                return bytes_read;
            }
        }
    }
    return -1; // Timeout
}

void r307s_init(void) {
    // A UART já foi inicializada
}

bool r307s_verify_password(void) {
    uint8_t packet = {0x13, (r307s_password >> 24) & 0xFF, (r307s_password >> 16) & 0xFF, (r307s_password >> 8) & 0xFF, r307s_password & 0xFF};
    write_packet(0x01, packet, 5);
    
    uint8_t response[4];
    if (read_packet(response, 12, 500) > 0) {
        return response[5] == R307S_OK;
    }
    return false;
}

uint8_t r307s_get_image(void) {
    uint8_t packet = {0x01};
    write_packet(0x01, packet, 1);
    
    uint8_t response[4];
    if (read_packet(response, 12, 1000) > 0) {
        return response[5];
    }
    return R307S_ERROR_COMM;
}

uint8_t r307s_image_to_tz(uint8_t slot) {
    uint8_t packet = {0x02, slot};
    write_packet(0x01, packet, 2);
    
    uint8_t response[4];
    if (read_packet(response, 12, 1000) > 0) {
        return response[5];
    }
    return R307S_ERROR_COMM;
}

uint8_t r307s_search_finger(uint16_t *finger_id, uint16_t *score) {
    uint8_t packet = {0x04, 0x01, 0x00, 0x00, 0x00, 0xA3}; // Search buffer 1 from 0 to 163
    write_packet(0x01, packet, 6);
    
    uint8_t response[6];
    if (read_packet(response, 16, 1000) > 0) {
        if (response[5] == R307S_OK) {
            *finger_id = (response[7] << 8) | response[8];
            *score = (response[4] << 8) | response[9];
        }
        return response[5];
    }
    return R307S_ERROR_COMM;
}