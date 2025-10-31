// src/include/hal/hal_spi_rp2040.h
#ifndef HAL_SPI_RP2040_H
#define HAL_SPI_RP2040_H

#include "hal/hal_spi.h"

/**
 * @brief Retorna a interface HAL SPI configurada para o Display ST7796.
 * 
 * Esta interface está configurada com os pinos específicos do display
 * (CS, DC, RST) e deve ser usada exclusivamente pelo driver do display.
 * 
 * @return Ponteiro constante para a interface HAL SPI do display.
 */
const hal_spi_interface_t* hal_spi_get_display_interface(void);

/**
 * @brief Retorna a interface HAL SPI configurada para o Cartão SD.
 * 
 * Esta interface está configurada com o pino CS específico do cartão SD
 * e deve ser usada exclusivamente pelo driver do SD card.
 * 
 * @return Ponteiro constante para a interface HAL SPI do SD card.
 */
const hal_spi_interface_t* hal_spi_get_sdcard_interface(void);

#endif // HAL_SPI_RP2040_H