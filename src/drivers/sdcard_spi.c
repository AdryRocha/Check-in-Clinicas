#include "sdcard_spi.h"
#include "hw_config.h" // Arquivo de configuração para a biblioteca FatFs
#include "sd_card.h"

// Instância global da estrutura do sistema de arquivos
static FATFS fs_inst;

// Este arquivo atua como a camada de configuração para a biblioteca `no-OS-FatFS-SD-SPI-RPi-Pico`.
// A implementação real do driver SPI e da lógica de bloco está dentro da biblioteca.
// O arquivo `hw_config.c` (não mostrado aqui, mas parte do projeto) define as
// estruturas `sd_card_t` e `spi_t` com os pinos corretos (SD_MISO, SD_CS, etc.).

bool sd_card_init(FATFS *fs) {
    // A função sd_init_driver() é fornecida pela biblioteca e lê a configuração de hw_config.c
    if (!sd_init_driver()) {
        // Erro ao inicializar o driver
        return false;
    }

    // Monta o sistema de arquivos no drive "0:"
    FRESULT fr = f_mount(fs, "0:", 1);
    if (fr!= FR_OK) {
        // Erro ao montar o sistema de arquivos
        return false;
    }

    return true;
}