#ifndef SDCARD_SPI_H
#define SDCARD_SPI_H

#include <stdbool.h>
#include "ff.h" // Header principal do FatFs

// Definições de pinos conforme o mapa de IO 
#define SD_SPI      spi1
#define SD_MISO     12
#define SD_CS       13
#define SD_SCK      14
#define SD_MOSI     15

/**
 * @brief Inicializa o driver do cartão SD e monta o sistema de arquivos.
 * 
 * @param fs Ponteiro para a estrutura do sistema de arquivos FatFs.
 * @return true se a inicialização e montagem forem bem-sucedidas, false caso contrário.
 */
bool sd_card_init(FATFS *fs);

#endif // SDCARD_SPI_H