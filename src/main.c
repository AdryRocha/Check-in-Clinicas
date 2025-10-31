// src/main.c
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "lvgl.h"

#include "include/system_init.h"
#include "ui/lvgl_port.h"
#include "ui/ui_manager.h"
#include "core/event_handler.h"
#include "core/state_machine.h"
#include "services/data_storage.h"
#include "drivers/gm67_qr.h"
#include "drivers/r307s_fingerprint.h"
#include "utils/error_handler.h"
#include "hal/hal_spi_rp2040.h"
#include "drivers/ST7796_display.h"

// Função de entrada para o Core 1 (UI)
void core1_entry(void) {
    while (1) {
        lv_timer_handler();
        sleep_ms(5);
    }
}

// Função principal no Core 0 (Lógica da Aplicação)
int main(void) {
    // 1. Inicialização do sistema
    system_init();
    
    printf("\n=== Clinic Check-in System ===\n");
    printf("Version: 1.0.0\n");
    printf("MCU: RP2040\n\n");
    
    // 2. Inicializar event handler
    event_handler_init();
    
    // 3. Obter interfaces HAL SPI
    const hal_spi_interface_t* display_spi = hal_spi_get_display_interface();
    const hal_spi_interface_t* sdcard_spi = hal_spi_get_sdcard_interface();
    
    // 4. Inicializar display
    printf("Inicializando display...\n");
    st7796_init(display_spi);
    printf("Display inicializado!\n");
    
    // 5. Inicializar drivers e serviços
    printf("Inicializando armazenamento...\n");
    if (!storage_init()) {
        printf("AVISO: Falha ao inicializar SD Card - continuando sem armazenamento\n");
    } else {
        printf("SD Card inicializado!\n");
    }
    
    printf("Inicializando leitor QR...\n");
    gm67_init();
    printf("Leitor QR inicializado!\n");
    
    printf("Inicializando sensor biométrico...\n");
    r307s_init();
    if (!r307s_verify_password()) {
        printf("AVISO: Falha na verificação de senha do sensor biométrico\n");
    } else {
        printf("Sensor biométrico verificado!\n");
    }
    
    // 6. Inicializar UI
    printf("Inicializando interface gráfica...\n");
    lvgl_port_init();
    ui_manager_init();
    printf("Interface gráfica inicializada!\n");
    
    // 7. Lançar o Core 1 para a UI
    printf("Lançando Core 1 para UI...\n");
    multicore_launch_core1(core1_entry);
    
    // 8. Inicializar a máquina de estados
    printf("Inicializando máquina de estados...\n");
    state_machine_init();
    
    // 9. Postar evento inicial
    event_t start_event = {.type = EV_SYSTEM_START};
    event_post(&start_event);
    
    printf("\n=== Sistema pronto! ===\n\n");
    
    // 10. Loop principal da aplicação
    char qr_buffer[256];
    uint32_t last_print = 0;
    
    while (1) {
        // Verifica o leitor de QR code
        int bytes_read = gm67_read_data(qr_buffer, sizeof(qr_buffer));
        if (bytes_read > 0) {
            qr_buffer[strcspn(qr_buffer, "\r\n")] = 0;
            
            event_t qr_event;
            qr_event.type = EV_QR_CODE_SCANNED;
            strncpy(qr_event.payload.qr_data, qr_buffer, sizeof(qr_event.payload.qr_data) - 1);
            qr_event.payload.qr_data[sizeof(qr_event.payload.qr_data) - 1] = '\0';
            
            event_post(&qr_event);
            
            printf("QR Code lido: %s\n", qr_buffer);
        }

        // Processamento de Eventos
        state_machine_run();

        // Print de status a cada 10 segundos
        uint32_t now = to_ms_since_boot(get_absolute_time());
        if (now - last_print > 10000) {
            last_print = now;
            printf("Sistema operacional - Uptime: %lu segundos\n", now / 1000);
        }

        sleep_ms(20);
    }

    return 0;
}