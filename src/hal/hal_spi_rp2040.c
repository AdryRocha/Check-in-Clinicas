#include "hal/hal_spi.h"
#include "hardware_config.h" // Onde seus pinos estão definidos
#include "hardware/spi.h"
#include "hardware/gpio.h"

// --- Funções de implementação para o SPI0 do RP2040 ---

static void rp2040_spi0_init(void) {
    // A inicialização principal já é feita em system_init.c
    // Esta função pode ser usada para configurações adicionais se necessário.
}

static void rp2040_spi0_write(const uint8_t* data, size_t len) {
    spi_write_blocking(SPI_PORT, data, len);
}

static void rp2040_spi0_read(uint8_t* buffer, size_t len) {
    spi_read_blocking(SPI_PORT, 0xFF, buffer, len);
}

// --- Funções específicas para o Display (usa pinos de CS, DC, RST do display) ---

static void rp2040_disp_set_cs(bool active) {
    gpio_put(PIN_DISP_CS,!active); // CS é ativo baixo
}

static void rp2040_disp_set_dc(bool is_data) {
    gpio_put(PIN_DISP_DC, is_data);
}

static void rp2040_disp_set_rst(bool active) {
    gpio_put(PIN_DISP_RST,!active); // RST é ativo baixo
}

// --- Funções específicas para o Cartão SD (usa apenas o pino CS do SD) ---

static void rp2040_sd_set_cs(bool active) {
    gpio_put(PIN_SD_CS,!active); // CS é ativo baixo
}


// --- Instâncias das Interfaces ---

// Interface para o Display ST7796
static const hal_spi_interface_t spi_display_interface = {
   .init = rp2040_spi0_init,
   .write = rp2040_spi0_write,
   .read = rp2040_spi0_read,
   .set_cs = rp2040_disp_set_cs,
   .set_dc = rp2040_disp_set_dc,
   .set_rst = rp2040_disp_set_rst,
};

// Interface para o Cartão SD
static const hal_spi_interface_t spi_sdcard_interface = {
   .init = rp2040_spi0_init,
   .write = rp2040_spi0_write,
   .read = rp2040_spi0_read,
   .set_cs = rp2040_sd_set_cs,
   .set_dc = NULL, // Cartão SD não usa pino DC
   .set_rst = NULL, // Cartão SD não usa pino RST
};


// --- Funções Públicas para obter as interfaces ---

const hal_spi_interface_t* hal_spi_get_display_interface(void) {
    return &spi_display_interface;
}

const hal_spi_interface_t* hal_spi_get_sdcard_interface(void) {
    return &spi_sdcard_interface;
}