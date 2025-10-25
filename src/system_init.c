#include "pico/stdlib.h"
#include "st7789_display.h"
#include "gm67_qr.h"
#include "r307s_fingerprint.h"
#include "data_storage.h"
#include "lvgl_port.h"
#include "ui_manager.h"
#include "network_sync.h"
#include "clinic_checkin_config.h" // Arquivo com SSID e senha

void system_init() {
    // Inicializa stdio para depuração via USB
    stdio_init_all();
    sleep_ms(2000); // Aguarda a conexão serial

    // Inicializa o hardware do display
    st7789_init();

    // Inicializa a biblioteca LVGL e a UI
    lvgl_port_init();
    ui_init();

    // Inicializa o leitor de QR code
    gm67_init();

    // Inicializa o sensor de impressão digital
    r307s_init();
    r307s_verify_password(); // Handshake com o sensor

    // Inicializa o serviço de armazenamento (monta o SD Card)
    if (!storage_init()) {
        // Tratar falha crítica na montagem do SD Card
        ui_show_error_screen("Falha no SD Card");
        while(1);
    }
    
    // Conecta ao Wi-Fi e sincroniza o tempo
    if (sync_connect_wifi(WIFI_SSID, WIFI_PASSWORD)) {
        rtc_init();
        sync_update_rtc_from_ntp();
    }
}