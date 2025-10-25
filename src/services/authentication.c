#include "authentication.h"
#include "r307s_fingerprint.h"
#include <string.h>
#include <stdio.h>

#define FP_MATCH_THRESHOLD 75 // Limiar de confiança para a correspondência da digital

bool auth_parse_qr_code(const char *qr_data, char *cpf_out) {
    const char *cpf_tag = "CPF:";
    char *start = strstr(qr_data, cpf_tag);
    if (!start) {
        return false;
    }
    start += strlen(cpf_tag);

    char *end = strchr(start, ';');
    if (!end) {
        end = qr_data + strlen(qr_data);
    }

    size_t len = end - start;
    if (len!= 11) { // CPF deve ter 11 dígitos
        return false;
    }

    strncpy(cpf_out, start, len);
    cpf_out[len] = '\0';
    return true;
}

appointment_record_t* auth_find_appointment(const char *cpf, appointment_record_t *records, uint16_t count) {
    for (uint16_t i = 0; i < count; i++) {
        if (strncmp(cpf, records[i].cpf, 11) == 0) {
            return &records[i];
        }
    }
    return NULL;
}

bool auth_verify_fingerprint(uint8_t template_id) {
    // Passo 1: Capturar a imagem
    if (r307s_capture_image()!= R307S_OK) {
        return false;
    }

    // Passo 2: Criar um template a partir da imagem
    if (r307s_create_template()!= R307S_OK) {
        return false;
    }

    // Passo 3: Comparar o template capturado com o template armazenado
    uint16_t confidence;
    if (r307s_match_template(template_id, &confidence)) {
        return confidence > FP_MATCH_THRESHOLD;
    }

    return false;
}