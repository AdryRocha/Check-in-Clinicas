#include "network_sync.h"
#include "pico/cyw43_arch.h"
#include "lwip/dns.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"

// Implementação de um cliente NTP simples para o Pico W
// (Adaptado de exemplos do SDK do Pico)

typedef struct NTP_T_ {
    ip_addr_t ntp_server_address;
    bool dns_request_sent;
    struct udp_pcb *ntp_pcb;
    absolute_time_t ntp_test_time;
    alarm_id_t ntp_resend_alarm;
} NTP_T;

#define NTP_SERVER "pool.ntp.org"
#define NTP_MSG_LEN 48
#define NTP_PORT 123
#define NTP_DELTA 2208988800 // seconds between 1 Jan 1900 and 1 Jan 1970

static NTP_T *ntp_state;

// Função de callback para a resposta NTP
static void ntp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port) {
    NTP_T *state = (NTP_T *)arg;
    uint8_t mode = pbuf_get_at(p, 0) & 0x7;
    uint8_t stratum = pbuf_get_at(p, 1);

    if (ip_addr_cmp(addr, &state->ntp_server_address) && port == NTP_PORT && p->tot_len == NTP_MSG_LEN &&
        mode == 0x4 && stratum!= 0) {
        uint8_t seconds_buf = {0};
        pbuf_copy_partial(p, seconds_buf, sizeof(seconds_buf), 40);
        uint32_t seconds_since_1900 = seconds_buf << 24 | seconds_buf << 16 | seconds_buf << 8 | seconds_buf;
        uint32_t seconds_since_1970 = seconds_since_1900 - NTP_DELTA;
        time_t epoch = seconds_since_1970;
        
        // Configura o RTC do Pico
        struct tm *utc = gmtime(&epoch);
        datetime_t dt = {
           .year  = (uint16_t)(utc->tm_year + 1900),
           .month = (uint8_t)(utc->tm_mon + 1),
           .day   = (uint8_t)utc->tm_mday,
           .dotw  = (uint8_t)utc->tm_wday,
           .hour  = (uint8_t)utc->tm_hour,
           .min   = (uint8_t)utc->tm_min,
           .sec   = (uint8_t)utc->tm_sec
        };
        rtc_set_datetime(&dt);
    }
    pbuf_free(p);
}

// Função para enviar a requisição NTP
static void ntp_request(NTP_T *state) {
    cyw43_arch_lwip_begin();
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, NTP_MSG_LEN, PBUF_RAM);
    uint8_t *req = (uint8_t *)p->payload;
    memset(req, 0, NTP_MSG_LEN);
    req = 0x1b;
    udp_sendto(state->ntp_pcb, p, &state->ntp_server_address, NTP_PORT);
    pbuf_free(p);
    cyw43_arch_lwip_end();
}

// Função de callback para a resolução de DNS
static void ntp_dns_found(const char *hostname, const ip_addr_t *ipaddr, void *arg) {
    NTP_T *state = (NTP_T *)arg;
    if (ipaddr) {
        state->ntp_server_address = *ipaddr;
        ntp_request(state);
    }
}

bool sync_connect_wifi(const char *ssid, const char *pass) {
    if (cyw43_arch_init()) {
        return false;
    }
    cyw43_arch_enable_sta_mode();
    
    if (cyw43_arch_wifi_connect_timeout_ms(ssid, pass, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        return false;
    }
    return true;
}

bool sync_update_rtc_from_ntp() {
    ntp_state = calloc(1, sizeof(NTP_T));
    if (!ntp_state) return false;

    ntp_state->ntp_pcb = udp_new();
    if (!ntp_state->ntp_pcb) {
        free(ntp_state);
        return false;
    }
    udp_recv(ntp_state->ntp_pcb, ntp_recv, ntp_state);

    cyw43_arch_lwip_begin();
    int err = dns_gethostbyname(NTP_SERVER, &ntp_state->ntp_server_address, ntp_dns_found, ntp_state);
    cyw43_arch_lwip_end();

    if (err == ERR_OK) {
        ntp_request(ntp_state); // DNS já estava em cache
    } else if (err!= ERR_INPROGRESS) {
        free(ntp_state);
        return false;
    }
    
    // Aguarda um tempo para a resposta NTP
    sleep_ms(5000);
    
    // Limpeza
    udp_remove(ntp_state->ntp_pcb);
    free(ntp_state);

    return true;
}

bool sync_fetch_daily_data() {
    // Placeholder para a implementação do cliente HTTP
    // 1. Conectar ao Wi-Fi (se não estiver conectado)
    // 2. Fazer uma requisição HTTP GET para o endpoint do servidor da clínica
    // 3. Receber o arquivo 'appointments.dat'
    // 4. Salvar o arquivo no cartão SD, sobrescrevendo o antigo
    return true;
}