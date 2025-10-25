#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "event_handler.h"
#include "data_storage.h"

// Enumeração de todos os estados possíveis da máquina de check-in
typedef enum {
    STATE_IDLE,
    STATE_QR_SCANNING,
    STATE_QR_VALIDATING,
    STATE_FP_WAITING,
    STATE_FP_SCANNING,
    STATE_FP_MATCHING,
    STATE_SUCCESS,
    STATE_ERROR,
} checkin_state_t;

// Estrutura de contexto para manter o estado da sessão de check-in atual
typedef struct {
    checkin_state_t current_state;
    uint32_t state_entry_time_ms;
    char qr_data;
    appointment_record_t current_appointment;
    char error_message;
} checkin_context_t;

/**
 * @brief Inicializa a máquina de estados.
 * 
 * @param ctx Ponteiro para o contexto da máquina de estados.
 */
void checkin_state_machine_init(checkin_context_t *ctx);

/**
 * @brief Processa um evento e atualiza o estado da máquina.
 * 
 * @param ctx Ponteiro para o contexto da máquina de estados.
 * @param event O evento a ser processado.
 */
void checkin_process_event(checkin_context_t *ctx, system_event_t event);

/**
 * @brief Executa ações contínuas dentro do estado atual (ex: timeouts, processamento).
 * 
 * @param ctx Ponteiro para o contexto da máquina de estados.
 */
void checkin_update_state(checkin_context_t *ctx);

#endif // STATE_MACHINE_H