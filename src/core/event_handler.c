#include "event_handler.h"
#include "pico/multicore.h"
#include "gm67_qr.h"
#include "r307s_fingerprint.h"

void core1_entry() {
    while (1) {
        // Verifica se há dados do leitor de QR code
        if (gm67_has_data()) {
            // Envia um evento para o Core 0 indicando que um QR code foi lido
            multicore_fifo_push_blocking(EVENT_QR_DETECTED);
            // Aguarda um pouco para evitar múltiplos eventos para a mesma leitura
            sleep_ms(500); 
        }

        // Verifica se um dedo foi colocado no sensor
        // Nota: A implementação de r307s_is_finger_present é bloqueante e não ideal
        // para polling. Uma solução de produção usaria um pino de interrupção
        // ou um modo de detecção de baixo consumo do sensor, se disponível.
        // if (r307s_is_finger_present()) {
        //     multicore_fifo_push_blocking(EVENT_FP_DETECTED);
        //     sleep_ms(500);
        // }
        
        // Pequeno atraso para não sobrecarregar o barramento
        sleep_ms(20);
    }
}