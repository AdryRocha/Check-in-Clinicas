#include "error_handler.h"
#include "ui_manager.h"
#include <stdio.h>

void handle_error(error_code_t error, const char *message) {
    // Log do erro no console serial para depuração
    printf("ERRO [%04X]: %s\n", error, message);

    // Ação baseada no tipo de erro
    switch (error & 0xF000) {
        case 0x1000: // Erros de Hardware
            // Para erros críticos de hardware, pode ser necessário reiniciar ou entrar em um estado de falha segura.
            ui_show_error_screen(message);
            // Em uma falha crítica como SD_MOUNT_FAILED, o sistema poderia entrar em um loop infinito.
            // while(1) { sleep_ms(1000); }
            break;
        
        case 0x3000: // Erros de Aplicação
            // Erros de aplicação geralmente são temporários e o sistema pode retornar ao estado IDLE.
            ui_show_error_screen(message);
            break;
        
        default:
            break;
    }
}