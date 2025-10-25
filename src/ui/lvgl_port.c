#include "lvgl_port.h"
#include "lvgl.h"
#include "st7789_display.h"

// Buffers de desenho para o LVGL
#define BUF_SIZE (DISPLAY_WIDTH * 20) // Buffer para 20 linhas
static lv_color_t buf1;
static lv_color_t buf2;

// Descritores de display e buffer
static lv_disp_draw_buf_t draw_buf;
static lv_disp_drv_t disp_drv;

// Callback do timer para o tick do LVGL
static bool lv_tick_callback(struct repeating_timer *t) {
    lv_tick_inc(5); // Informa ao LVGL que 5ms se passaram
    return true;
}

void lvgl_port_init() {
    // Inicializa o LVGL
    lv_init();

    // Inicializa os buffers de desenho
    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, BUF_SIZE);

    // Inicializa e registra o driver do display
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = DISPLAY_WIDTH;
    disp_drv.ver_res = DISPLAY_HEIGHT;
    disp_drv.flush_cb = st7789_flush_cb;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // Configura o timer de hardware para o tick do LVGL
    static struct repeating_timer timer;
    add_repeating_timer_ms(5, lv_tick_callback, NULL, &timer);
}