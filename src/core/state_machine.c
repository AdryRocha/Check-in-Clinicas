#include "state_machine.h"
#include "ui_manager.h"
#include "authentication.h"
#include "gm67_qr.h"
#include <stdio.h>

#define STATE_TIMEOUT_MS 15000 // Timeout geral de 15 segundos
#define SUCCESS_ERROR_DISPLAY_MS 3000 // Tempo de exibição de mensagens de sucesso/erro

// Array global de agendamentos carregados na memória
static appointment_record_t daily_appointments;
static uint16_t appointment_count = 0;

// Função auxiliar para mudar de estado
static void set_state(checkin_context_t *ctx, checkin_state_t new_state) {
    ctx->current_state = new_state;
    ctx->state_entry_time_ms = to_ms_since_boot(get_absolute_time());
}

void checkin_state_machine_init(checkin_context_t *ctx) {
    // Carrega os agendamentos do dia do SD Card para a RAM
    if (!storage_load_daily_appointments(daily_appointments, &appointment_count)) {
        // Tratar erro de leitura do SD Card
        set_state(ctx, STATE_ERROR);
        strncpy(ctx->error_message, "Erro no SD Card", sizeof(ctx->error_message));
        ui_show_error_screen(ctx->error_message);
    } else {
        set_state(ctx, STATE_IDLE);
        ui_show_welcome_screen();
    }
}

void checkin_process_event(checkin_context_t *ctx, system_event_t event) {
    switch (ctx->current_state) {
        case STATE_IDLE:
            if (event == EVENT_QR_DETECTED) {
                set_state(ctx, STATE_QR_SCANNING);
                ui_show_scanning_qr_screen();
            }
            break;

        case STATE_FP_WAITING:
            if (event == EVENT_FP_DETECTED) {
                set_state(ctx, STATE_FP_SCANNING);
                ui_show_scanning_fp_screen();
            }
            break;
        
        default:
            // Eventos ignorados em outros estados
            break;
    }
}

void checkin_update_state(checkin_context_t *ctx) {
    uint32_t time_in_state = to_ms_since_boot(get_absolute_time()) - ctx->state_entry_time_ms;

    switch (ctx->current_state) {
        case STATE_QR_SCANNING:
            if (gm67_read_qr(ctx->qr_data, sizeof(ctx->qr_data))) {
                set_state(ctx, STATE_QR_VALIDATING);
            }
            break;

        case STATE_QR_VALIDATING:
            {
                char cpf;
                if (auth_parse_qr_code(ctx->qr_data, cpf)) {
                    appointment_record_t *appt = auth_find_appointment(cpf, daily_appointments, appointment_count);
                    if (appt) {
                        ctx->current_appointment = *appt;
                        set_state(ctx, STATE_FP_WAITING);
                        ui_show_place_finger_screen();
                    } else {
                        set_state(ctx, STATE_ERROR);
                        strncpy(ctx->error_message, "Agendamento nao encontrado", sizeof(ctx->error_message));
                        ui_show_error_screen(ctx->error_message);
                    }
                } else {
                    set_state(ctx, STATE_ERROR);
                    strncpy(ctx->error_message, "QR Code Invalido", sizeof(ctx->error_message));
                    ui_show_error_screen(ctx->error_message);
                }
            }
            break;

        case STATE_FP_SCANNING:
            // A verificação da digital é bloqueante, então a transição é imediata
            if (auth_verify_fingerprint(ctx->current_appointment.fp_template_id)) {
                storage_log_checkin(&ctx->current_appointment);
                set_state(ctx, STATE_SUCCESS);
                ui_show_success_screen();
                // Acionar LED/Buzzer de sucesso aqui
            } else {
                set_state(ctx, STATE_ERROR);
                strncpy(ctx->error_message, "Digital nao reconhecida", sizeof(ctx->error_message));
                ui_show_error_screen(ctx->error_message);
            }
            break;

        case STATE_SUCCESS:
        case STATE_ERROR:
            if (time_in_state > SUCCESS_ERROR_DISPLAY_MS) {
                set_state(ctx, STATE_IDLE);
                ui_show_welcome_screen();
            }
            break;

        default:
            break;
    }

    // Verificação de timeout geral
    if (ctx->current_state!= STATE_IDLE && ctx->current_state!= STATE_SUCCESS && ctx->current_state!= STATE_ERROR) {
        if (time_in_state > STATE_TIMEOUT_MS) {
            set_state(ctx, STATE_ERROR);
            strncpy(ctx->error_message, "Tempo esgotado", sizeof(ctx->error_message));
            ui_show_error_screen(ctx->error_message);
        }
    }
}