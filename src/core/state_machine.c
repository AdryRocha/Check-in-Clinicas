#include "state_machine.h"
#include "ui_manager.h" // Assumindo que ui_manager existe
#include "authentication.h" // Assumindo que authentication existe
#include <stdio.h>

static system_state_t current_state = STATE_INIT;

void state_machine_init(void) {
    current_state = STATE_IDLE;
    // Posta um evento inicial para começar
    event_t start_event = {.type = EV_SYSTEM_START};
    event_post(&start_event);
}

void state_machine_run(void) {
    event_t current_event;
    if (!event_get(&current_event)) {
        return; // Nenhum evento para processar
    }

    printf("State: %d, Event: %d\n", current_state, current_event.type);

    switch (current_state) {
        case STATE_IDLE:
            if (current_event.type == EV_SYSTEM_START) {
                ui_manager_show_welcome_screen();
                current_state = STATE_AWAITING_QR;
            }
            break;

        case STATE_AWAITING_QR:
            if (current_event.type == EV_QR_CODE_SCANNED) {
                ui_manager_show_prompt_screen("QR Recebido", "Aproxime o dedo...");
                auth_start_for_qr(current_event.payload.qr_data);
                current_state = STATE_AWAITING_FINGERPRINT;
            }
            break;

        case STATE_AWAITING_FINGERPRINT:
            if (current_event.type == EV_FINGER_DETECTED) {
                ui_manager_show_message("Processando...");
                if (auth_check_fingerprint()) {
                    event_t success_event = {.type = EV_AUTH_SUCCESS};
                    event_post(&success_event);
                } else {
                    event_t failure_event = {.type = EV_AUTH_FAILURE};
                    event_post(&failure_event);
                }
                current_state = STATE_AUTHENTICATING;
            }
            break;
        
        case STATE_AUTHENTICATING:
            if (current_event.type == EV_AUTH_SUCCESS) {
                ui_manager_show_success_screen("Check-in Realizado!");
                current_state = STATE_CHECKIN_SUCCESS;
            } else if (current_event.type == EV_AUTH_FAILURE) {
                ui_manager_show_failure_screen("Falha na Autenticação");
                current_state = STATE_CHECKIN_FAILURE;
            }
            break;

        //... outros estados e transições
        
        default:
            // Lógica para estados de sucesso, falha e erro, retornando para IDLE
            current_state = STATE_IDLE;
            ui_manager_show_welcome_screen();
            break;
    }
}