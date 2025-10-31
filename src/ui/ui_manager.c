#include "ui/ui_manager.h"
#include "lvgl.h"

// Funções auxiliares para criar elementos de UI
static void create_screen_with_label(const char* text) {
    lv_obj_clean(lv_scr_act());
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, text);
    lv_obj_center(label);
}

void ui_manager_init(void) {
    // A inicialização da LVGL é feita no lvgl_port
}

void ui_manager_show_welcome_screen(void) {
    create_screen_with_label("Clinica Check-in\nAproxime o QR Code");
}

void ui_manager_show_prompt_screen(const char* title, const char* message) {
    lv_obj_clean(lv_scr_act());
    lv_obj_t *title_label = lv_label_create(lv_scr_act());
    lv_label_set_text(title_label, title);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 50);

    lv_obj_t *msg_label = lv_label_create(lv_scr_act());
    lv_label_set_text(msg_label, message);
    lv_obj_center(msg_label);
}

void ui_manager_show_message(const char* message) {
    create_screen_with_label(message);
}

void ui_manager_show_success_screen(const char* message) {
    lv_obj_clean(lv_scr_act());
    lv_obj_set_style_bg_color(lv_scr_act(), lv_palette_main(LV_PALETTE_GREEN), LV_PART_MAIN);
    create_screen_with_label(message);
}

void ui_manager_show_failure_screen(const char* message) {
    lv_obj_clean(lv_scr_act());
    lv_obj_set_style_bg_color(lv_scr_act(), lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);
    create_screen_with_label(message);
}

// Implementação do lv_conf.h
// Este arquivo deve estar em src/include/ui/lv_conf.h
/*
#ifndef LV_CONF_H
#define LV_CONF_H

#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 1

#define LV_USE_LOG 1
#define LV_LOG_PRINTF 1

//... outras configurações da LVGL...

#endif
*/