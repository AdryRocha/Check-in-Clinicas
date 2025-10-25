#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include "data_storage.h"
#include <stdbool.h>

/**
 * @brief Analisa a string do QR code para extrair o CPF.
 * Formato esperado: "CPF:<11_digitos>;..." 
 * 
 * @param qr_data A string completa lida do QR code.
 * @param cpf_out Buffer para armazenar o CPF extraído (deve ter pelo menos 12 bytes).
 * @return true se o formato for válido e o CPF for extraído, false caso contrário.
 */
bool auth_parse_qr_code(const char *qr_data, char *cpf_out);

/**
 * @brief Procura por um agendamento correspondente ao CPF na lista de agendamentos.
 * 
 * @param cpf O CPF a ser procurado.
 * @param records O array de registros de agendamento.
 * @param count O número de registros no array.
 * @return Um ponteiro para o registro encontrado, ou NULL se não for encontrado.
 */
appointment_record_t* auth_find_appointment(const char *cpf, appointment_record_t *records, uint16_t count);

/**
 * @brief Orquestra o processo de verificação da impressão digital.
 * 
 * @param template_id O ID do template a ser verificado no sensor.
 * @return true se a verificação for bem-sucedida, false caso contrário.
 */
bool auth_verify_fingerprint(uint8_t template_id);

#endif // AUTHENTICATION_H