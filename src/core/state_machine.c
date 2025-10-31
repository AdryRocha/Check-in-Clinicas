// state_machine.c (IMPLEMENTAÇÃO NECESSÁRIA)
#include "core/state_machine.h"
#include "event_handler.h"
#include "ui_manager.h"
#include "authentication.h"
#include "network_sync.h"
#include "pico/stdlib.h"
#include <stdio.h>

static system_state_t current_state = STATE_INIT;
static uint32_t state_timeout_ms = 0;
static uint32_t state_entry_time = 0;

// Timeouts por estado (em ms)
#define TIMEOUT_QR_SCAN         30000   // 30 segundos
#define TIMEOUT_FINGERPRINT     15000   // 15 segundos
#define TIMEOUT_AUTH            5000    // 5 segundos
#define TIMEOUT_SUCCESS_SCREEN  3000    // 3 segundos
#define TIMEOUT_FAILURE_SCREEN  3000    // 3 segundos

static void enter_state(system_state_t new_state);
static void handle_event_in_state(const event_t* event);
static void check_timeout(void);

void state_machine_init(void) {
    current_state = STATE_INIT;
    enter_state(STATE_IDLE);
}

void state_machine_run(void) {
    event_t event;
    
    // Processar eventos pendentes
    while (event_get(&event)) {
        handle_event_in_state(&event);
    }
    
    // Verificar timeout
    check_timeout();
}

static void enter_state(system_state_t new_state) {
    current_state = new_state;
    state_entry_time = to_ms_since_boot(get_absolute_time());
    
    printf("Estado: %d\n", new_state);
    
    switch (new_state) {
        case STATE_IDLE:
            ui_manager_show_welcome_screen();
            state_timeout_ms = 0;  // Sem timeout
            break;
            
        case STATE_AWAITING_QR:
            ui_manager_show_prompt_screen("CHECK-IN", "Aproxime o QR Code");
            state_timeout_ms = TIMEOUT_QR_SCAN;
            break;
            
        case STATE_AWAITING_FINGERPRINT:
            ui_manager_show_prompt_screen("AUTENTICAÇÃO", "Coloque o dedo no sensor");
            state_timeout_ms = TIMEOUT_FINGERPRINT;
            break;
            
        case STATE_AUTHENTICATING:
            ui_manager_show_message("Verificando...");
            state_timeout_ms = TIMEOUT_AUTH;
            break;
            
        case STATE_CHECKIN_SUCCESS:
            ui_manager_show_success_screen("CHECK-IN REALIZADO!\nBem-vindo(a)");
            state_timeout_ms = TIMEOUT_SUCCESS_SCREEN;
            break;
            
        case STATE_CHECKIN_FAILURE:
            ui_manager_show_failure_screen("FALHA NA AUTENTICAÇÃO\nTente novamente");
            state_timeout_ms = TIMEOUT_FAILURE_SCREEN;
            break;
            
        case STATE_ERROR:
            ui_manager_show_failure_screen("ERRO DO SISTEMA\nContate o suporte");
            state_timeout_ms = 5000;
            break;
            
        default:
            break;
    }
}

static void handle_event_in_state(const event_t* event) {
    switch (current_state) {
        case STATE_IDLE:
            if (event->type == EV_SYSTEM_START) {
                enter_state(STATE_AWAITING_QR);
            }
            break;
            
        case STATE_AWAITING_QR:
            if (event->type == EV_QR_CODE_SCANNED) {
                if (auth_start_for_qr(event->payload.qr_data)) {
                    enter_state(STATE_AWAITING_FINGERPRINT);
                } else {
                    enter_state(STATE_CHECKIN_FAILURE);
                }
            }
            break;
            
        case STATE_AWAITING_FINGERPRINT:
            if (event->type == EV_FINGER_DETECTED) {
                enter_state(STATE_AUTHENTICATING);
                
                // Verificar autenticação
                if (auth_check_fingerprint()) {
                    event_t success_event = {.type = EV_AUTH_SUCCESS};
                    event_post(&success_event);
                } else {
                    event_t failure_event = {.type = EV_AUTH_FAILURE};
                    event_post(&failure_event);
                }
            }
            break;
            
        case STATE_AUTHENTICATING:
            if (event->type == EV_AUTH_SUCCESS) {
                // TODO: Enviar para servidor
                enter_state(STATE_CHECKIN_SUCCESS);
            } else if (event->type == EV_AUTH_FAILURE) {
                enter_state(STATE_CHECKIN_FAILURE);
            }
            break;
            
        case STATE_CHECKIN_SUCCESS:
        case STATE_CHECKIN_FAILURE:
        case STATE_ERROR:
            // Estes estados retornam ao IDLE após timeout
            break;
            
        default:
            break;
    }
}

static void check_timeout(void) {
    if (state_timeout_ms == 0) {
        return;  // Estado sem timeout
    }
    
    uint32_t elapsed = to_ms_since_boot(get_absolute_time()) - state_entry_time;
    
    if (elapsed >= state_timeout_ms) {
        printf("Timeout no estado %d\n", current_state);
        
        switch (current_state) {
            case STATE_AWAITING_QR:
            case STATE_AWAITING_FINGERPRINT:
            case STATE_AUTHENTICATING:
                enter_state(STATE_CHECKIN_FAILURE);
                break;
                
            case STATE_CHECKIN_SUCCESS:
            case STATE_CHECKIN_FAILURE:
            case STATE_ERROR:
                enter_state(STATE_IDLE);
                break;
                
            default:
                break;
        }
    }
}