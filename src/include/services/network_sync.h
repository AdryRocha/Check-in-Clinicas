#ifndef NETWORK_SYNC_H
#define NETWORK_SYNC_H

#include <stdbool.h>
#include "pico/util/datetime.h"

/**
 * @brief Conecta à rede Wi-Fi.
 * 
 * @param ssid O SSID da rede.
 * @param pass A senha da rede.
 * @return true se a conexão for bem-sucedida, false caso contrário.
 */
bool sync_connect_wifi(const char *ssid, const char *pass);

/**
 * @brief Busca os dados de agendamento do servidor (placeholder).
 * Esta função irá, em uma implementação futura, baixar 'appointments.dat' via HTTP.
 * 
 * @return true se a busca for bem-sucedida, false caso contrário.
 */
bool sync_fetch_daily_data();

/**
 * @brief Sincroniza o tempo do sistema com um servidor NTP.
 * 
 * @return true se a sincronização for bem-sucedida, false caso contrário.
 */
bool sync_update_rtc_from_ntp();

#endif // NETWORK_SYNC_H