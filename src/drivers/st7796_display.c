// src/drivers/ST7796_display.c
#include "drivers/ST7796_display.h"
#include "hal/hal_spi.h"
#include "hardware_config.h"
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/spi.h"

// Forward declarations das funções privadas
static void st7796_write_cmd(uint8_t cmd);
static void st7796_write_data_byte(uint8_t data);
static void dma_handler(void);

// Variáveis estáticas
static const hal_spi_interface_t* spi_interface = NULL;
static int dma_channel = -1;
static volatile bool dma_busy = false;

void st7796_init(const hal_spi_interface_t* interface) {
    if (interface == NULL) {
        return;  // Proteção contra ponteiro nulo
    }
    
    spi_interface = interface;
    
    // Setup DMA
    dma_channel = dma_claim_unused_channel(true);
    
    // Reset do display
    spi_interface->set_rst(false);  // Ativa reset (ativo baixo)
    sleep_ms(10);
    spi_interface->set_rst(true);   // Desativa reset
    sleep_ms(10);
    spi_interface->set_rst(false);  // Ativa reset novamente
    sleep_ms(10);
    spi_interface->set_rst(true);   // Desativa reset
    sleep_ms(120);
    
    // Sequência de inicialização do ST7796
    st7796_write_cmd(0x01);  // Software Reset
    sleep_ms(120);
    
    st7796_write_cmd(0x11);  // Sleep Out
    sleep_ms(120);
    
    // Configuração do formato de pixel
    st7796_write_cmd(0x3A);  // Pixel Format Set
    st7796_write_data_byte(0x55);  // 16-bit color (RGB565)
    
    // Configuração de orientação da memória
    st7796_write_cmd(0x36);  // Memory Access Control
    st7796_write_data_byte(0x48);  // Row/Column exchange, RGB order
    
    // Configurações adicionais recomendadas para ST7796
    st7796_write_cmd(0xF0);  // Command Set Control
    st7796_write_data_byte(0xC3);
    
    st7796_write_cmd(0xF0);
    st7796_write_data_byte(0x96);
    
    st7796_write_cmd(0xB4);  // Display Inversion Control
    st7796_write_data_byte(0x01);
    
    st7796_write_cmd(0xB6);  // Display Function Control
    st7796_write_data_byte(0x80);
    st7796_write_data_byte(0x02);
    st7796_write_data_byte(0x3B);
    
    st7796_write_cmd(0xE8);  // Display Output Ctrl Adjust
    st7796_write_data_byte(0x40);
    st7796_write_data_byte(0x8A);
    st7796_write_data_byte(0x00);
    st7796_write_data_byte(0x00);
    st7796_write_data_byte(0x29);
    st7796_write_data_byte(0x19);
    st7796_write_data_byte(0xA5);
    st7796_write_data_byte(0x33);
    
    st7796_write_cmd(0xC5);  // VCOM Control
    st7796_write_data_byte(0x27);
    
    st7796_write_cmd(0xC2);  // Power Control 3
    st7796_write_data_byte(0xA7);
    
    st7796_write_cmd(0xE0);  // Positive Gamma Control
    st7796_write_data_byte(0xF0);
    st7796_write_data_byte(0x09);
    st7796_write_data_byte(0x0B);
    st7796_write_data_byte(0x06);
    st7796_write_data_byte(0x04);
    st7796_write_data_byte(0x15);
    st7796_write_data_byte(0x2F);
    st7796_write_data_byte(0x54);
    st7796_write_data_byte(0x42);
    st7796_write_data_byte(0x3C);
    st7796_write_data_byte(0x17);
    st7796_write_data_byte(0x14);
    st7796_write_data_byte(0x18);
    st7796_write_data_byte(0x1B);
    
    st7796_write_cmd(0xE1);  // Negative Gamma Control
    st7796_write_data_byte(0xF0);
    st7796_write_data_byte(0x09);
    st7796_write_data_byte(0x0B);
    st7796_write_data_byte(0x06);
    st7796_write_data_byte(0x04);
    st7796_write_data_byte(0x03);
    st7796_write_data_byte(0x2D);
    st7796_write_data_byte(0x43);
    st7796_write_data_byte(0x42);
    st7796_write_data_byte(0x3B);
    st7796_write_data_byte(0x16);
    st7796_write_data_byte(0x14);
    st7796_write_data_byte(0x17);
    st7796_write_data_byte(0x1B);
    
    st7796_write_cmd(0xF0);
    st7796_write_data_byte(0x3C);
    
    st7796_write_cmd(0xF0);
    st7796_write_data_byte(0x69);
    
    sleep_ms(120);
    
    st7796_write_cmd(0x29);  // Display ON
    sleep_ms(50);
}

static void st7796_write_cmd(uint8_t cmd) {
    if (spi_interface == NULL) return;
    
    spi_interface->set_cs(true);
    spi_interface->set_dc(false);  // Modo comando
    spi_interface->write(&cmd, 1);
    spi_interface->set_cs(false);
}

static void st7796_write_data_byte(uint8_t data) {
    if (spi_interface == NULL) return;
    
    spi_interface->set_cs(true);
    spi_interface->set_dc(true);  // Modo dados
    spi_interface->write(&data, 1);
    spi_interface->set_cs(false);
}

void st7796_set_window(uint16_t x_start, uint16_t y_start, 
                       uint16_t x_end, uint16_t y_end) {
    // Column Address Set
    st7796_write_cmd(0x2A);
    st7796_write_data_byte(x_start >> 8);
    st7796_write_data_byte(x_start & 0xFF);
    st7796_write_data_byte(x_end >> 8);
    st7796_write_data_byte(x_end & 0xFF);
    
    // Page Address Set
    st7796_write_cmd(0x2B);
    st7796_write_data_byte(y_start >> 8);
    st7796_write_data_byte(y_start & 0xFF);
    st7796_write_data_byte(y_end >> 8);
    st7796_write_data_byte(y_end & 0xFF);
    
    // Memory Write
    st7796_write_cmd(0x2C);
}

void st7796_write_data(const uint8_t *data, size_t len) {
    if (spi_interface == NULL || data == NULL) return;
    
    spi_interface->set_cs(true);
    spi_interface->set_dc(true);
    spi_interface->write(data, len);
    spi_interface->set_cs(false);
}

static void dma_handler(void) {
    dma_hw->ints0 = 1u << dma_channel;  // Clear interrupt
    
    if (spi_interface != NULL) {
        spi_interface->set_cs(false);
    }
    
    dma_busy = false;
}

void st7796_write_data_dma(const uint8_t *data, size_t len) {
    if (spi_interface == NULL || data == NULL || len == 0) return;
    
    dma_busy = true;
    
    spi_interface->set_cs(true);
    spi_interface->set_dc(true);
    
    dma_channel_config cfg = dma_channel_get_default_config(dma_channel);
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_8);
    channel_config_set_dreq(&cfg, spi_get_dreq(SPI_PORT, true));
    
    dma_channel_set_irq0_enabled(dma_channel, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    irq_set_enabled(DMA_IRQ_0, true);
    
    dma_channel_configure(
        dma_channel,
        &cfg,
        &spi_get_hw(SPI_PORT)->dr,
        data,
        len,
        true
    );
}

bool st7796_is_dma_busy(void) {
    return dma_busy;
}