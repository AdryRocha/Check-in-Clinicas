#include "utils/error_handler.h"
#include "pico/stdlib.h"
#include "lvgl.h"
#include <stdio.h>

// Tenta usar a UI para exibir o erro, se a LVGL estiver disponível.
static void display_error_message(const char* file, int line, const char* msg) {
    if (lv_is_initialized()) {
        lv_obj_t *scr = lv_scr_act();
        if(scr) {
            lv_obj_clean(scr);
            lv_obj_set_style_bg_color(scr, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);
            
            lv_obj_t *label = lv_label_create(scr);
            lv_label_set_text_fmt(label, "FATAL ERROR\nFile: %s\nLine: %d\nCond: %s", file, line, msg);
            lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
            lv_obj_center(label);
        }
    }
}

void error_handler(const char* file, int line, const char* msg) {
    // 1. Desabilita interrupções para evitar mais corrupção de estado.
    uint32_t ints = save_and_disable_interrupts();

    // 2. Tenta exibir a mensagem de erro na tela.
    display_error_message(file, line, msg);

    // 3. Imprime a mensagem de erro no console de depuração.
    printf("!!! FATAL ERROR!!!\n");
    printf("File: %s, Line: %d\n", file, line);
    printf("Condition failed: %s\n", msg);

    // 4. Entra em um loop infinito para parar o sistema.
    // Em um sistema de produção, isso poderia piscar um LED de erro.
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    while (1) {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        sleep_ms(100);
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        sleep_ms(100);
    }
    
    // Restaura as interrupções (nunca será alcançado)
    restore_interrupts(ints);
}