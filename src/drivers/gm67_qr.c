#include "gm67_qr.h"
#include "hardware_config.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"

// Comando para acionar a leitura (pode variar com o modelo/configuração)
static const uint8_t CMD_TRIGGER_SCAN = {0x7E, 0x00, 0x08, 0x01, 0x00, 0x02, 0x01, 0xAB, 0xCD};

void gm67_init(void) {
    // A inicialização da UART já foi feita em system_init
}

void gm67_trigger_scan(void) {
    uart_write_blocking(UART_QR_ID, CMD_TRIGGER_SCAN, sizeof(CMD_TRIGGER_SCAN));
}

int gm67_read_data(char *buffer, size_t buffer_len) {
    int bytes_read = 0;
    if (uart_is_readable(UART_QR_ID)) {
        while (uart_is_readable_within_us(UART_QR_ID, 10000) && bytes_read < buffer_len - 1) {
            buffer[bytes_read++] = uart_getc(UART_QR_ID);
        }
    }
    buffer[bytes_read] = '\0'; // Nul-terminate the string
    return bytes_read;
}