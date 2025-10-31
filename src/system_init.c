#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h" // Para set_sys_clock_khz
#include "hardware/rtc.h"    // Para rtc_init
#include "include/hardware_config.h" // Para PIN_SPI_SCK, SPI_PORT, WIFI_SSID, etc.
#include "services/network_sync.h"  // Para sync_connect_wifi
//#include "st7796_display.h"
//#include "gm67_qr.h"
//#include "r307s_fingerprint.h"
//#include "data_storage.h"
//#include "lvgl_port.h"
//#include "ui_manager.h"
//#include "network_sync.h"
//#include "clinic_checkin_config.h" // Arquivo com SSID e senha

void system_init(void) {
    // 1. Configura o clock do sistema para 250 MHz para melhor desempenho.
    // A frequência exata pode ser ajustada conforme necessário.
    set_sys_clock_khz(250000, true);

    // 2. Inicializa a E/S padrão sobre USB para depuração.
    stdio_usb_init();

    // Aguarda um momento para a conexão USB ser estabelecida, opcional.
    // sleep_ms(2000); 

    // 3. Inicializa os periféricos de hardware.
    
    // --- SPI0 para Display e Cartão SD ---
    spi_init(SPI_PORT, 10 * 1000 * 1000); // Inicializa com 10 MHz
    gpio_set_function(PIN_SPI_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SPI_TX, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SPI_RX, GPIO_FUNC_SPI);
    // Pinos CS serão controlados manualmente como GPIO

    // --- UART0 para Leitor de QR Code ---
    uart_init(UART_QR_ID, UART_QR_BAUD_RATE);
    gpio_set_function(PIN_QR_TX, GPIO_FUNC_UART);
    gpio_set_function(PIN_QR_RX, GPIO_FUNC_UART);

    // --- UART1 para Sensor de Impressão Digital ---
    uart_init(UART_FP_ID, UART_FP_BAUD_RATE);
    gpio_set_function(PIN_FP_TX, GPIO_FUNC_UART);
    gpio_set_function(PIN_FP_RX, GPIO_FUNC_UART);

    // --- GPIOs de Controle ---
    // Display ST7796
    gpio_init(PIN_DISP_CS);
    gpio_set_dir(PIN_DISP_CS, GPIO_OUT);
    gpio_put(PIN_DISP_CS, 1); // CS em estado inativo (alto)

    gpio_init(PIN_DISP_DC);
    gpio_set_dir(PIN_DISP_DC, GPIO_OUT);
    
    gpio_init(PIN_DISP_RST);
    gpio_set_dir(PIN_DISP_RST, GPIO_OUT);

    gpio_init(PIN_DISP_BLK);
    gpio_set_dir(PIN_DISP_BLK, GPIO_OUT);
    gpio_put(PIN_DISP_BLK, 1); // Liga o backlight

    // Cartão SD
    gpio_init(PIN_SD_CS);
    gpio_set_dir(PIN_SD_CS, GPIO_OUT);
    gpio_put(PIN_SD_CS, 1); // CS em estado inativo (alto)

    
    // Conecta ao Wi-Fi e sincroniza o tempo
    if (sync_connect_wifi(WIFI_SSID, WIFI_PASSWORD)) {
        rtc_init();
        sync_update_rtc_from_ntp();
    }

