// src/include/drivers/ST7796_display.h
#ifndef ST7796_DISPLAY_H
#define ST7796_DISPLAY_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Forward declaration da interface HAL
struct hal_spi_interface;
typedef struct hal_spi_interface hal_spi_interface_t;

/**
 * @brief Inicializa o display ST7796.
 * @param interface Ponteiro para a interface HAL SPI a ser utilizada.
 */
void st7796_init(const hal_spi_interface_t* interface);

/**
 * @brief Define a janela de desenho no display.
 * @param x_start Coordenada X inicial.
 * @param y_start Coordenada Y inicial.
 * @param x_end Coordenada X final.
 * @param y_end Coordenada Y final.
 */
void st7796_set_window(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end);

/**
 * @brief Escreve dados no display (modo bloqueante).
 * @param data Ponteiro para o buffer de dados.
 * @param len Número de bytes a serem escritos.
 */
void st7796_write_data(const uint8_t *data, size_t len);

/**
 * @brief Escreve dados no display usando DMA (modo não-bloqueante).
 * @param data Ponteiro para o buffer de dados.
 * @param len Número de bytes a serem escritos.
 */
void st7796_write_data_dma(const uint8_t *data, size_t len);

/**
 * @brief Verifica se o DMA está ocupado.
 * @return true se o DMA está em uso, false caso contrário.
 */
bool st7796_is_dma_busy(void);

#endif // ST7796_DISPLAY_H