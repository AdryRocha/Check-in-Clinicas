#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "lvgl.h"

#include "system_init.h"
#include "lvgl_port.h"
#include "ui_manager.h"
#include "event_handler.h"
#include "state_machine.h"
#include "data_storage.h"
#include "gm67_qr.h"
#include "r307s_fingerprint.h"
#include "error_handler.h"

// Função de entrada para o Core 1 (UI)
void core1_entry() {
    while (1) {
        // O handler da LVGL deve ser chamado continuamente
        lv_timer_handler();
        sleep_ms(5);
    }
}

// Função principal no Core 0 (Lógica da Aplicação)
int main() {
    // 1. Inicialização do sistema
    system_init();
    event_handler_init();
    
    // Inicializa os drivers e serviços
    ASSERT(storage_init());
    gm67_init();
    r307s_init();
    ASSERT(r307s_verify_password());

    // Inicializa a UI
    lvgl_port_init();
    ui_manager_init();

    // 2. Lança o Core 1 para a UI
    multicore_launch_core1(core1_entry);

    // 3. Inicializa a máquina de estados
    state_machine_init();

    // 4. Loop principal da aplicação
    char qr_buffer;
    while (1) {
        // --- Polling de Periféricos (Produtores de Eventos) ---
        
        // Verifica o leitor de QR code
        int bytes_read = gm67_read_data(qr_buffer, sizeof(qr_buffer));
        if (bytes_read > 0) {
            // Limpa caracteres de nova linha
            qr_buffer[strcspn(qr_buffer, "\r\n")] = 0;
            
            event_t qr_event;
            qr_event.type = EV_QR_CODE_SCANNED;
            strncpy(qr_event.payload.qr_data, qr_buffer, sizeof(qr_event.payload.qr_data) - 1);
            event_post(&qr_event);
        }

        // Verifica o sensor de impressão digital (exemplo simplificado)
        // Em um sistema real, isso seria acionado pelo estado
        // if (some_condition_to_check_finger()) {
        //     if (r307s_get_image() == R307S_OK) {
        //         event_t fp_event = {.type = EV_FINGER_DETECTED};
        //         event_post(&fp_event);
        //     }
        // }

        // --- Processamento de Eventos (Consumidor) ---
        state_machine_run();

        // Pequeno atraso para não sobrecarregar a CPU
        sleep_ms(20);
    }

    return 0;
}