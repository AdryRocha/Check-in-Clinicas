#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

// Códigos de erro conforme especificação 
typedef enum {
    ERR_NONE = 0,
    // Erros de Hardware
    ERR_QR_INIT_FAILED = 0x1001,
    ERR_FP_INIT_FAILED = 0x1010,
    ERR_FP_MATCH_FAILED = 0x1012,
    ERR_SD_MOUNT_FAILED = 0x1020,
    ERR_SD_READ_ERROR = 0x1021,
    // Erros de Aplicação
    ERR_INVALID_QR = 0x3001,
    ERR_APPOINTMENT_NOT_FOUND = 0x3002,
} error_code_t;

/**
 * @brief Manipula um erro do sistema.
 * 
 * @param error O código do erro a ser tratado.
 * @param message Mensagem descritiva opcional.
 */
void handle_error(error_code_t error, const char *message);

#endif // ERROR_HANDLER_H