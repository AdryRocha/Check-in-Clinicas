// src/include/core/event_handler.h
#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Tipos de eventos do sistema.
 */
typedef enum {
    EV_NONE,
    EV_SYSTEM_START,
    EV_QR_CODE_SCANNED,
    EV_FINGER_DETECTED,
    EV_AUTH_SUCCESS,
    EV_AUTH_FAILURE,
    EV_TIMEOUT,
    // Adicionar mais eventos conforme necessário
} event_type_t;

/**
 * @brief Estrutura representando um evento do sistema.
 */
typedef struct {
    event_type_t type;
    union {
        char qr_data[256];      // CORRIGIDO: array ao invés de char único
        uint16_t finger_id;
    } payload;
} event_t;

/**
 * @brief Inicializa o sistema de gerenciamento de eventos.
 */
void event_handler_init(void);

/**
 * @brief Posta um evento na fila.
 * @param event Ponteiro para o evento a ser postado.
 */
void event_post(const event_t *event);

/**
 * @brief Obtém um evento da fila.
 * @param event Ponteiro onde o evento será armazenado.
 * @return true se um evento foi obtido, false se a fila está vazia.
 */
bool event_get(event_t *event);

#endif // EVENT_HANDLER_H