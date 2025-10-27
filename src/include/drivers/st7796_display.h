#ifndef ST7796_DISPLAY_H
#define ST7796_DISPLAY_H

#include <stdint.h>
#include <stddef.h>

void st7796_init(void);
void st7796_set_window(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end);
void st7796_write_data(const uint8_t *data, size_t len);
void st7796_write_data_dma(const uint8_t *data, size_t len);
bool st7796_is_dma_busy(void);

#endif // ST7796_DISPLAY_H