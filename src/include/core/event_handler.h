#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

// Enumeração de todos os eventos possíveis no sistema
typedef enum {
    EVENT_NONE,
    EVENT_QR_DETECTED,
    EVENT_FP_DETECTED,
    EVENT_TIMEOUT,
} system_event_t;

/**
 * @brief Ponto de entrada para o Core 1.
 * Este loop infinito é responsável por sondar os sensores e enviar eventos para o Core 0.
 */
void core1_entry();

#endif // EVENT_HANDLER_H