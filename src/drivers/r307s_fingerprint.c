// r307s_fingerprint.c (IMPLEMENTAÇÃO NECESSÁRIA)
#include "drivers/r307s_fingerprint.h"
#include "hardware_config.h"
#include "hardware/uart.h"
#include "pico/stdlib.h"
#include <string.h>

#define R307S_HEADER_H      0xEF
#define R307S_HEADER_L      0x01
#define R307S_DEFAULT_ADDR  0xFFFFFFFF
#define R307S_DEFAULT_PASS  0x00000000

// Comandos
#define R307S_CMD_GENIMAGE      0x01
#define R307S_CMD_IMAGE2TZ      0x02
#define R307S_CMD_MATCH         0x03
#define R307S_CMD_SEARCH        0x04
#define R307S_CMD_REGMODEL      0x05
#define R307S_CMD_STORE         0x06
#define R307S_CMD_LOAD          0x07
#define R307S_CMD_UPLOAD        0x08
#define R307S_CMD_DOWNLOAD      0x09
#define R307S_CMD_DELETCHAR     0x0C
#define R307S_CMD_EMPTY         0x0D
#define R307S_CMD_SETPASSWORD   0x12
#define R307S_CMD_VERIFYPASSWORD 0x13
#define R307S_CMD_TEMPLATECOUNT 0x1D

typedef struct {
    uint8_t header[2];      // 0xEF, 0x01
    uint8_t address[4];     // Default: 0xFF 0xFF 0xFF 0xFF
    uint8_t type;           // 0x01=cmd, 0x02=data, 0x07=ack
    uint8_t length[2];      // Comprimento do payload + checksum
    uint8_t payload[64];    // Dados variáveis
} r307s_packet_t;

static uint16_t calc_checksum(const uint8_t* data, size_t len) {
    uint16_t sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += data[i];
    }
    return sum;
}

static bool send_packet(uint8_t cmd, const uint8_t* params, size_t param_len) {
    r307s_packet_t pkt;
    
    pkt.header[0] = R307S_HEADER_H;
    pkt.header[1] = R307S_HEADER_L;
    
    pkt.address[0] = 0xFF;
    pkt.address[1] = 0xFF;
    pkt.address[2] = 0xFF;
    pkt.address[3] = 0xFF;
    
    pkt.type = 0x01;  // Command
    
    uint16_t length = 3 + param_len;  // cmd + params + checksum(2)
    pkt.length[0] = (length >> 8) & 0xFF;
    pkt.length[1] = length & 0xFF;
    
    pkt.payload[0] = cmd;
    if (params && param_len > 0) {
        memcpy(&pkt.payload[1], params, param_len);
    }
    
    // Calcular checksum
    uint16_t checksum = pkt.type + pkt.length[0] + pkt.length[1] + cmd;
    for (size_t i = 0; i < param_len; i++) {
        checksum += params[i];
    }
    
    pkt.payload[1 + param_len] = (checksum >> 8) & 0xFF;
    pkt.payload[2 + param_len] = checksum & 0xFF;
    
    // Enviar pacote
    size_t total_len = 9 + 1 + param_len + 2;
    uart_write_blocking(UART_FP_ID, (const uint8_t*)&pkt, total_len);
    
    return true;
}

static uint8_t receive_ack(uint8_t* data, size_t* data_len, uint32_t timeout_ms) {
    uint32_t start = to_ms_since_boot(get_absolute_time());
    uint8_t buffer[128];
    size_t idx = 0;
    
    while ((to_ms_since_boot(get_absolute_time()) - start) < timeout_ms) {
        if (uart_is_readable(UART_FP_ID)) {
            buffer[idx++] = uart_getc(UART_FP_ID);
            
            // Verificar header
            if (idx >= 9) {
                if (buffer[0] == R307S_HEADER_H && buffer[1] == R307S_HEADER_L) {
                    uint16_t len = (buffer[7] << 8) | buffer[8];
                    
                    if (idx >= (9 + len)) {
                        // Pacote completo recebido
                        uint8_t confirm_code = buffer[9];
                        
                        if (data && data_len) {
                            *data_len = len - 3;  // Excluir confirm_code e checksum
                            memcpy(data, &buffer[10], *data_len);
                        }
                        
                        return confirm_code;
                    }
                }
            }
        }
        sleep_us(100);
    }
    
    return R307S_ERROR_COMM;
}

void r307s_init(void) {
    // UART já inicializado em system_init
    sleep_ms(500);  // Aguardar sensor inicializar
}

bool r307s_verify_password(void) {
    uint8_t params[4] = {0x00, 0x00, 0x00, 0x00};  // Senha padrão
    send_packet(R307S_CMD_VERIFYPASSWORD, params, 4);
    
    uint8_t ack = receive_ack(NULL, NULL, 1000);
    return (ack == R307S_OK);
}

uint8_t r307s_get_image(void) {
    send_packet(R307S_CMD_GENIMAGE, NULL, 0);
    return receive_ack(NULL, NULL, 5000);
}

uint8_t r307s_image_to_tz(uint8_t slot) {
    uint8_t params[1] = {slot};
    send_packet(R307S_CMD_IMAGE2TZ, params, 1);
    return receive_ack(NULL, NULL, 3000);
}

uint8_t r307s_search_finger(uint16_t *finger_id, uint16_t *score) {
    uint8_t params[5] = {
        0x01,           // CharBuffer
        0x00, 0x00,     // Start page
        0x00, 0xFF      // Page num (255)
    };
    
    send_packet(R307S_CMD_SEARCH, params, 5);
    
    uint8_t data[16];
    size_t data_len;
    uint8_t ack = receive_ack(data, &data_len, 3000);
    
    if (ack == R307S_OK && data_len >= 4) {
        *finger_id = (data[0] << 8) | data[1];
        *score = (data[2] << 8) | data[3];
    }
    
    return ack;
}