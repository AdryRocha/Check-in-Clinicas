#ifndef R307S_FINGERPRINT_H
#define R307S_FINGERPRINT_H

#include "pico/stdlib.h"
#include <stdbool.h>

// Definições de pinos e UART conforme o mapa de IO 
#define FP_UART_ID      uart1
#define FP_TX_PIN       4
#define FP_RX_PIN       5
#define FP_BAUD_RATE    57600

// Códigos de confirmação do sensor
#define R307S_OK                    0x00
#define R307S_PACKET_ERROR          0x01
#define R307S_NO_FINGER             0x02
#define R307S_IMAGE_FAIL            0x03
#define R307S_IMAGE_MESS            0x06
#define R307S_FEATURE_FAIL          0x07
#define R307S_NO_MATCH              0x08
#define R307S_NOT_FOUND             0x09
#define R307S_ENROLL_MISMATCH       0x0A
#define R307S_BAD_LOCATION          0x0B
#define R307S_DB_RANGE_FAIL         0x0C
#define R307S_UPLOAD_FEATURE_FAIL   0x0D
#define R307S_PACKET_RESPONSE_FAIL  0x0E
#define R307S_UPLOAD_FAIL           0x0F
#define R307S_DB_IS_FULL            0x11
#define R307S_PASSWORD_FAIL         0x13
#define R307S_INVALID_IMAGE         0x15

/**
 * @brief Inicializa a interface UART para o sensor de impressão digital.
 */
void r307s_init();

/**
 * @brief Verifica a senha do sensor (handshake).
 * 
 * @return true se a verificação for bem-sucedida, false caso contrário.
 */
bool r307s_verify_password();

/**
 * @brief Detecta se um dedo está presente no sensor.
 * 
 * @return true se um dedo for detectado, false caso contrário.
 */
bool r307s_is_finger_present();

/**
 * @brief Captura a imagem da impressão digital e a armazena no buffer interno do sensor.
 * 
 * @return Código de confirmação do sensor (e.g., R307S_OK).
 */
uint8_t r307s_capture_image();

/**
 * @brief Gera um template (características) a partir da imagem no buffer e o armazena no CharBuffer1.
 * 
 * @return Código de confirmação do sensor.
 */
uint8_t r307s_create_template();

/**
 * @brief Compara o template no CharBuffer1 com um template armazenado na memória flash do sensor.
 * 
 * @param template_id O ID do template a ser comparado (0-999).
 * @param confidence Ponteiro para armazenar a pontuação de confiança da correspondência.
 * @return true se houver correspondência, false caso contrário.
 */
bool r307s_match_template(uint8_t template_id, uint16_t *confidence);

#endif // R307S_FINGERPRINT_H