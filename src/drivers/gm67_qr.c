#include "gm67_qr.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include <string.h>

#define QR_BUFFER_SIZE 256

// Buffer circular para recebimento de dados via interrupção
static volatile char rx_buffer;
static volatile int rx_write_index = 0;
static volatile int rx_read_index = 0;
static volatile bool full_message_received = false;

// Rotina de Serviço de Interrupção (ISR) da UART
static void on_uart_rx() {
    while (uart_is_readable(QR_UART_ID)) {
        char ch = uart_getc(QR_UART_ID);

        // Armazena o caractere no buffer se houver espaço
        int next_write_index = (rx_write_index + 1) % QR_BUFFER_SIZE;
        if (next_write_index!= rx_read_index) {
            rx_buffer[rx_write_index] = ch;
            rx_write_index = next_write_index;
        }

        // Verifica se a mensagem terminou (CR ou LF)
        if (ch == '\n' |

| ch == '\r') {
            full_message_received = true;
        }
    }
}

void gm67_init() {
    uart_init(QR_UART_ID, QR_BAUD_RATE);
    gpio_set_function(QR_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(QR_RX_PIN, GPIO_FUNC_UART);
    
    // Configura a interrupção de recebimento
    int uart_irq = (QR_UART_ID == uart0)? UART0_IRQ : UART1_IRQ;
    irq_set_exclusive_handler(uart_irq, on_uart_rx);
    irq_set_enabled(uart_irq, true);
    uart_set_irq_enables(QR_UART_ID, true, false);
}

bool gm67_has_data() {
    return full_message_received;
}

size_t gm67_read_qr(char *buffer, size_t len) {
    if (!full_message_received) {
        return 0;
    }

    size_t count = 0;
    // Desabilita interrupções temporariamente para acesso seguro ao buffer
    uint32_t status = save_and_disable_interrupts();

    while (rx_read_index!= rx_write_index && count < (len - 1)) {
        char ch = rx_buffer[rx_read_index];
        rx_read_index = (rx_read_index + 1) % QR_BUFFER_SIZE;
        
        // Ignora caracteres de nova linha
        if (ch!= '\n' && ch!= '\r') {
            buffer[count++] = ch;
        }
    }
    buffer[count] = '\0'; // Termina a string

    // Reseta o estado
    full_message_received = false;
    rx_read_index = 0;
    rx_write_index = 0;

    restore_interrupts(status);
    return count;
}