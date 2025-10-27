#include "lvgl_port.h"
#include "lvgl.h"
#include "hardware_config.h"
#include "ST7796_display.h"
#include "pico/stdlib.h"

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1; // Buffer de 20 linhas
static lv_disp_drv_t disp_drv;

static void disp_flush_cb(lv_disp_drv_t *disp_driver, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    
    st7796_set_window(area->x1, area->y1, area->x2, area->y2);
    st7796_write_data_dma((uint8_t *)color_p, w * h * sizeof(lv_color_t));
    
    // Informa à LVGL que o flush foi concluído (de forma assíncrona)
    // O callback do DMA irá realmente sinalizar o fim.
    // Para LVGL, precisamos chamar lv_disp_flush_ready imediatamente.
    lv_disp_flush_ready(disp_driver);
}

static bool lv_tick_timer_callback(struct repeating_timer *t) {
    LV_UNUSED(t);
    lv_tick_inc(5); // Informa à LVGL que 5ms se passaram
    return true;
}

void lvgl_port_init(void) {
    lv_init();
    
    st7796_init();
    
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, DISP_WIDTH * 20);
    
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = DISP_WIDTH;
    disp_drv.ver_res = DISP_HEIGHT;
    disp_drv.flush_cb = disp_flush_cb;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
    
    // Configura o timer para o tick da LVGL
    static struct repeating_timer tick_timer;
    add_repeating_timer_ms(5, lv_tick_timer_callback, NULL, &tick_timer);
}