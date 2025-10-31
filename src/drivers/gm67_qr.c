// gm67_qr.c (IMPLEMENTAÇÃO NECESSÁRIA)
#include "drivers/gm67_qr.h"
#include "hardware_config.h"
#include "hardware/uart.h"
#include "pico/stdlib.h"
#include <string.h>

#define QR_BUFFER_SIZE 512

static char rx_buffer[QR_BUFFER_SIZE];
static volatile uint16_t rx_head = 0;
static volatile uint16_t rx_tail = 0;

// IRQ Handler para recepção de dados
static void on_uart_rx(void) {
    while (uart_is_readable(UART_QR_ID)) {
        uint8_t ch = uart_getc(UART_QR_ID);
        
        uint16_t next_head = (rx_head + 1) % QR_BUFFER_SIZE;
        if (next_head != rx_tail) {
            rx_buffer[rx_head] = ch;
            rx_head = next_head;
        }
    }
}

void gm67_init(void) {
    // UART já foi inicializado em system_init
    
    // Habilita interrupção RX
    uart_set_irq_enables(UART_QR_ID, true, false);
    irq_set_exclusive_handler(UART0_IRQ, on_uart_rx);
    irq_set_enabled(UART0_IRQ, true);
    
    // Configurar o GM67 para modo contínuo (se necessário)
    // Enviar comandos de configuração via UART
}

void gm67_trigger_scan(void) {
    // Se o módulo precisar de um trigger manual
    // Enviar comando apropriado
    const char cmd[] = {0x7E, 0x00, 0x08, 0x01, 0x00, 0x02, 0x01, 0xAB, 0xCD};
    uart_write_blocking(UART_QR_ID, (const uint8_t*)cmd, sizeof(cmd));
}

int gm67_read_data(char *buffer, size_t buffer_len) {
    if (rx_head == rx_tail) {
        return 0;  // Nenhum dado disponível
    }
    
    size_t bytes_copied = 0;
    
    // Copia dados do buffer circular até encontrar '\n' ou encher o buffer
    while (rx_tail != rx_head && bytes_copied < buffer_len - 1) {
        char ch = rx_buffer[rx_tail];
        rx_tail = (rx_tail + 1) % QR_BUFFER_SIZE;
        
        buffer[bytes_copied++] = ch;
        
        if (ch == '\n') {
            break;
        }
    }
    
    buffer[bytes_copied] = '\0';
    return bytes_copied;
}