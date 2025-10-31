// src/include/services/data_storage.h
#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Estrutura representando um usuário do sistema.
 */
typedef struct {
    char user_id[64];        // CORRIGIDO: array ao invés de char único
    uint16_t fingerprint_id;
    char name[128];          // CORRIGIDO: array ao invés de char único
} user_t;

/**
 * @brief Inicializa o sistema de armazenamento (SD Card).
 * @return true se inicializado com sucesso, false caso contrário.
 */
bool storage_init(void);

/**
 * @brief Busca um usuário pelo ID.
 * @param id ID do usuário (extraído do QR code).
 * @param user Ponteiro para estrutura onde os dados serão armazenados.
 * @return true se o usuário foi encontrado, false caso contrário.
 */
bool storage_find_user_by_id(const char* id, user_t* user);

/**
 * @brief Registra um check-in no log.
 * @param user_id ID do usuário.
 * @return true se o log foi gravado com sucesso, false caso contrário.
 */
bool storage_log_checkin(const char* user_id);

#endif // DATA_STORAGE_H