#ifndef ST7789_DISPLAY_H
#define ST7789_DISPLAY_H

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "lvgl.h"

// Definições de pinos conforme o mapa de IO 
#define DISPLAY_SPI       spi0
#define DISPLAY_MISO      16 // Não utilizado pelo display
#define DISPLAY_CS        17
#define DISPLAY_SCK       18
#define DISPLAY_MOSI      19
#define DISPLAY_RST       20
#define DISPLAY_DC        21

#define DISPLAY_WIDTH     120
#define DISPLAY_HEIGHT    240

/**
 * @brief Inicializa o hardware SPI e o display ST7789.
 * Envia a sequência de comandos de inicialização para o controlador do display.
 */
void st7789_init();

/**
 * @brief Função de callback para o LVGL. Envia o buffer de desenho para o display.
 * Esta função utiliza DMA para uma transferência de dados eficiente e não bloqueante.
 * 
 * @param disp_drv Ponteiro para o driver de display do LVGL.
 * @param area A área do display a ser atualizada.
 * @param color_p Ponteiro para o buffer de cores a ser enviado.
 */
void st7789_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);

#endif // ST7789_DISPLAY_H