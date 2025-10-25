#include "r307s_fingerprint.h"
#include "hardware/uart.h"
#include <string.h>

// Estrutura de pacote do R307S conforme especificação 
#pragma pack(1)
typedef struct {
    uint16_t header;
    uint32_t address;
    uint8_t type;
    uint16_t length;
    uint8_t data; // Payload máximo
} r307s_packet_t;
#pragma pack()

static const uint16_t R307S_HEADER = 0xEF01;
static const uint32_t R307S_ADDRESS = 0xFFFFFFFF;

// Função para enviar um pacote de comando
static void send_packet(uint8_t type, uint8_t *payload, uint16_t len) {
    uint8_t packet_buffer[sizeof(r307s_packet_t) + 2];
    r307s_packet_t *packet = (r307s_packet_t *)packet_buffer;

    packet->header = __builtin_bswap16(R307S_HEADER);
    packet->address = __builtin_bswap32(R307S_ADDRESS);
    packet->type = type;
    packet->length = __builtin_bswap16(len + 2); // Comprimento inclui checksum

    memcpy(packet->data, payload, len);

    uint16_t checksum = type + (len + 2);
    for (int i = 0; i < len; i++) {
        checksum += payload[i];
    }

    packet_buffer[sizeof(r307s_packet_t) - 32 + len] = (checksum >> 8) & 0xFF;
    packet_buffer[sizeof(r307s_packet_t) - 32 + len + 1] = checksum & 0xFF;

    uart_write_blocking(FP_UART_ID, packet_buffer, sizeof(r307s_packet_t) - 32 + len + 2);
}

// Função para receber um pacote de resposta
static bool receive_packet(r307s_packet_t *packet, uint32_t timeout_ms) {
    uint8_t buffer[sizeof(r307s_packet_t) + 2];
    uint64_t start_time = time_us_64();

    int bytes_read = 0;
    while (bytes_read < 9 && (time_us_64() - start_time) < (timeout_ms * 1000)) {
        if (uart_is_readable(FP_UART_ID)) {
            buffer[bytes_read++] = uart_getc(FP_UART_ID);
        }
    }

    if (bytes_read < 9) return false; // Timeout ou erro

    memcpy(packet, buffer, 9);
    packet->header = __builtin_bswap16(packet->header);
    packet->length = __builtin_bswap16(packet->length);

    if (packet->header!= R307S_HEADER) return false;

    uint16_t data_len = packet->length - 2;
    while (bytes_read < (9 + data_len + 2) && (time_us_64() - start_time) < (timeout_ms * 1000)) {
         if (uart_is_readable(FP_UART_ID)) {
            buffer[bytes_read++] = uart_getc(FP_UART_ID);
        }
    }

    if (bytes_read < (9 + data_len + 2)) return false; // Timeout

    memcpy(packet->data, buffer + 9, data_len);
    
    // Validação do checksum (simplificada para este exemplo)
    // Uma implementação de produção validaria o checksum aqui.
    
    return true;
}

void r307s_init() {
    uart_init(FP_UART_ID, FP_BAUD_RATE);
    gpio_set_function(FP_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(FP_RX_PIN, GPIO_FUNC_UART);
    sleep_ms(500); // Aguarda o sensor inicializar
}

bool r307s_verify_password() {
    uint8_t payload = {0x13, 0x00, 0x00, 0x00, 0x00}; // Comando VfyPwd
    send_packet(0x01, payload, sizeof(payload));
    
    r307s_packet_t response;
    if (receive_packet(&response, 1000) && response.data == R307S_OK) {
        return true;
    }
    return false;
}

bool r307s_is_finger_present() {
    uint8_t payload = {0x2B}; // Comando GenImg
    send_packet(0x01, payload, sizeof(payload));

    r307s_packet_t response;
    if (receive_packet(&response, 200)) {
        // O comando GenImg retorna OK se um dedo for detectado.
        // Se não houver dedo, ele retorna NO_FINGER após um timeout interno.
        // Para uma detecção não bloqueante, este método não é ideal.
        // A especificação  sugere uma verificação simples no Core 1,
        // o que implica que o sensor pode ter um pino de detecção ou um modo de polling rápido.
        // Para este driver, simulamos com um timeout curto.
        return response.data == R307S_OK;
    }
    return false;
}

uint8_t r307s_capture_image() {
    uint8_t payload = {0x01}; // Comando GenImg
    send_packet(0x01, payload, sizeof(payload));
    
    r307s_packet_t response;
    if (receive_packet(&response, 2000)) {
        return response.data;
    }
    return R307S_PACKET_RESPONSE_FAIL;
}

uint8_t r307s_create_template() {
    uint8_t payload = {0x02, 0x01}; // Comando Img2Tz no CharBuffer1
    send_packet(0x01, payload, sizeof(payload));
    
    r307s_packet_t response;
    if (receive_packet(&response, 1000)) {
        return response.data;
    }
    return R307S_PACKET_RESPONSE_FAIL;
}

bool r307s_match_template(uint8_t template_id, uint16_t *confidence) {
    uint8_t payload = {0x04, 0x01, (uint8_t)(template_id >> 8), (uint8_t)template_id}; // Comando Search
    send_packet(0x01, payload, sizeof(payload));

    r307s_packet_t response;
    if (receive_packet(&response, 1000) && response.data == R307S_OK) {
        if (confidence) {
            *confidence = (response.data << 8) | response.data;
        }
        return true;
    }
    return false;
}