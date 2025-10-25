#include "ui_manager.h"
#include "lvgl.h"

// Ponteiros estáticos para os objetos de tela (screens)
static lv_obj_t *screen_welcome;
static lv_obj_t *screen_scanning_qr;
static lv_obj_t *screen_place_finger;
static lv_obj_t *screen_scanning_fp;
static lv_obj_t *screen_success;
static lv_obj_t *screen_error;

// Ponteiro para o label de erro, para que a mensagem possa ser atualizada
static lv_obj_t *error_label;

// Função auxiliar para criar uma tela base com estilo padrão
static lv_obj_t* create_base_screen() {
    lv_obj_t *scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x003366), LV_PART_MAIN);
    return scr;
}

// Função auxiliar para criar um label centrado
static lv_obj_t* create_centered_label(lv_obj_t *parent, const char *text) {
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    return label;
}

void ui_init() {
    // Tela de Boas-Vindas
    screen_welcome = create_base_screen();
    create_centered_label(screen_welcome, "Bem-vindo!\nAproxime o QR Code.");

    // Tela de Leitura do QR Code
    screen_scanning_qr = create_base_screen();
    lv_obj_t *spinner_qr = lv_spinner_create(screen_scanning_qr, 1000, 60);
    lv_obj_set_size(spinner_qr, 100, 100);
    lv_obj_align(spinner_qr, LV_ALIGN_CENTER, 0, -20);
    create_centered_label(screen_scanning_qr, "Lendo QR Code...");

    // Tela "Coloque o Dedo"
    screen_place_finger = create_base_screen();
    create_centered_label(screen_place_finger, "Por favor, coloque o dedo\nno sensor.");

    // Tela de Leitura da Digital
    screen_scanning_fp = create_base_screen();
    lv_obj_t *spinner_fp = lv_spinner_create(screen_scanning_fp, 1000, 60);
    lv_obj_set_size(spinner_fp, 100, 100);
    lv_obj_align(spinner_fp, LV_ALIGN_CENTER, 0, -20);
    create_centered_label(screen_scanning_fp, "Lendo digital...");

    // Tela de Sucesso
    screen_success = create_base_screen();
    create_centered_label(screen_success, "Check-in realizado\ncom sucesso!");

    // Tela de Erro
    screen_error = create_base_screen();
    error_label = create_centered_label(screen_error, "Erro");
}

void ui_show_welcome_screen() {
    lv_scr_load(screen_welcome);
}

void ui_show_scanning_qr_screen() {
    lv_scr_load(screen_scanning_qr);
}

void ui_show_place_finger_screen() {
    lv_scr_load(screen_place_finger);
}

void ui_show_scanning_fp_screen() {
    lv_scr_load(screen_scanning_fp);
}

void ui_show_success_screen() {
    lv_scr_load(screen_success);
}

void ui_show_error_screen(const char *message) {
    lv_label_set_text(error_label, message);
    lv_scr_load(screen_error);
}