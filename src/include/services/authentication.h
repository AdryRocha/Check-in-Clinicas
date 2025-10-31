// src/include/services/authentication.h
#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include <stdbool.h>

/**
 * @brief Inicia o processo de autenticação para um QR code.
 * @param qr_data String contendo os dados do QR code.
 * @return true se o usuário foi encontrado, false caso contrário.
 */
bool auth_start_for_qr(const char* qr_data);

/**
 * @brief Verifica a impressão digital do usuário atual.
 * @return true se a digital foi autenticada, false caso contrário.
 */
bool auth_check_fingerprint(void);

#endif // AUTHENTICATION_H