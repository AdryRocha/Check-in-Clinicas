#ifndef GM67_QR_H
#define GM67_QR_H

#include <stddef.h>
#include <stdbool.h>

// Definições de pinos e UART conforme o mapa de IO 
#define QR_UART_ID      uart0
#define QR_TX_PIN       0
#define QR_RX_PIN       1
#define QR_BAUD_RATE    9600

/**
 * @brief Inicializa a interface UART para o leitor de QR code.
 * Configura a UART e a interrupção de recebimento.
 */
void gm67_init();

/**
 * @brief Verifica se há uma mensagem completa (terminada em '\n') no buffer.
 * 
 * @return true se uma mensagem completa está disponível, false caso contrário.
 */
bool gm67_has_data();

/**
 * @brief Lê a mensagem de QR code do buffer de recebimento.
 * 
 * @param buffer O buffer de destino para a string do QR code.
 * @param len O tamanho máximo do buffer de destino.
 * @return O número de bytes lidos, ou 0 se não houver dados.
 */
size_t gm67_read_qr(char *buffer, size_t len);

#endif // GM67_QR_H