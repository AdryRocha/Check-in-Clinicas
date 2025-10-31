// src/include/services/network_sync.h
#ifndef NETWORK_SYNC_H
#define NETWORK_SYNC_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Estrutura representando um registro de check-in.
 */
typedef struct {
    char user_id[64];        // CORRIGIDO: array ao invés de char único
    uint32_t timestamp;      // CORRIGIDO: uint32_t ao invés de uint64_t
    uint16_t fingerprint_score;
} checkin_record_t;

/**
 * @brief Sincroniza um registro de check-in com o servidor.
 * @param record Ponteiro para o registro a ser sincronizado.
 * @return true se a sincronização foi bem-sucedida, false caso contrário.
 */
bool network_sync_checkin_record(const checkin_record_t* record);

#endif // NETWORK_SYNC_H