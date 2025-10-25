#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#include "pico/stdlib.h"
#include <stdbool.h>

#define MAX_APPOINTMENTS 200 // Define um limite máximo de agendamentos por dia

// Estruturas de dados conforme a especificação 
typedef struct {
    char cpf;
    char crm;
    uint32_t appointment_time;
    uint16_t duration_minutes;
    uint8_t fp_template_id;
    uint8_t status;
    uint32_t checkin_time;
} appointment_record_t;

/**
 * @brief Inicializa o serviço de armazenamento.
 * 
 * @return true se bem-sucedido, false caso contrário.
 */
bool storage_init();

/**
 * @brief Carrega os registros de agendamento do arquivo 'appointments.dat'.
 * 
 * @param records Ponteiro para o array onde os registros serão armazenados.
 * @param count Ponteiro para a variável que receberá o número de registros lidos.
 * @return true se o carregamento for bem-sucedido, false caso contrário.
 */
bool storage_load_daily_appointments(appointment_record_t *records, uint16_t *count);

/**
 * @brief Registra um evento de check-in no arquivo 'checkins.log'.
 * 
 * @param record O registro do agendamento que resultou no check-in.
 * @return true se o registro for bem-sucedido, false caso contrário.
 */
bool storage_log_checkin(const appointment_record_t *record);

#endif // DATA_STORAGE_H