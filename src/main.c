#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "lvgl.h"
#include "system_init.h"
#include "state_machine.h"
#include "event_handler.h"

// Contexto da máquina de estados
static checkin_context_t checkin_ctx;

int main() {
    // Inicializa todos os sistemas e periféricos
    system_init();

    // Inicializa a máquina de estados
    checkin_state_machine_init(&checkin_ctx);

    // Lança o loop de I/O no Core 1
    multicore_launch_core1(core1_entry);

    // Loop principal no Core 0
    while (1) {
        // 1. Verifica por eventos do Core 1
        if (multicore_fifo_rvalid()) {
            system_event_t event = (system_event_t)multicore_fifo_pop_blocking();
            checkin_process_event(&checkin_ctx, event);
        }

        // 2. Atualiza a lógica interna da máquina de estados
        checkin_update_state(&checkin_ctx);

        // 3. Processa as tarefas do LVGL (desenho, animações, etc.)
        lv_timer_handler();

        // Pequeno atraso para ceder tempo de processamento
        sleep_ms(5);
    }

    return 0;
}