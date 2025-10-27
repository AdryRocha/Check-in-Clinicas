#include "ST7796_display.h"
#include "hardware_config.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"

static int dma_chan;
static volatile bool dma_busy = false;

// Função de callback para o DMA
static void dma_irq_handler() {
    if (dma_channel_get_irq0_status(dma_chan)) {
        dma_channel_acknowledge_irq0(dma_chan);
        dma_busy = false;
        gpio_put(PIN_DISP_CS, 1); // Libera CS após a transferência
    }
}

static void st7796_write_cmd(uint8_t cmd) {
    gpio_put(PIN_DISP_CS, 0);
    gpio_put(PIN_DISP_DC, 0); // Command mode
    spi_write_blocking(SPI_PORT, &cmd, 1);
    gpio_put(PIN_DISP_CS, 1);
}

static void st7796_write_data_byte(uint8_t data) {
    gpio_put(PIN_DISP_CS, 0);
    gpio_put(PIN_DISP_DC, 1); // Data mode
    spi_write_blocking(SPI_PORT, &data, 1);
    gpio_put(PIN_DISP_CS, 1);
}

void st7796_write_data(const uint8_t *data, size_t len) {
    gpio_put(PIN_DISP_CS, 0);
    gpio_put(PIN_DISP_DC, 1); // Data mode
    spi_write_blocking(SPI_PORT, data, len);
    gpio_put(PIN_DISP_CS, 1);
}

void st7796_init(void) {
    // Configura o canal DMA
    dma_chan = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_dreq(&c, spi_get_dreq(SPI_PORT, true));
    dma_channel_configure(dma_chan, &c, &spi_get_hw(SPI_PORT)->dr, NULL, 0, false);
    
    // Configura a interrupção do DMA
    dma_channel_set_irq0_enabled(dma_chan, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_irq_handler);
    irq_set_enabled(DMA_IRQ_0, true);

    // Sequência de reset do hardware
    gpio_put(PIN_DISP_RST, 1);
    sleep_ms(100);
    gpio_put(PIN_DISP_RST, 0);
    sleep_ms(100);
    gpio_put(PIN_DISP_RST, 1);
    sleep_ms(100);

    // Sequência de inicialização do ST7796
    st7796_write_cmd(0x01); // Software Reset
    sleep_ms(120);
    st7796_write_cmd(0x11); // Sleep Out
    sleep_ms(120);
    
    st7796_write_cmd(0x36); // Memory Access Control
    st7796_write_data_byte(0x28); // MY, MX, MV, ML, BGR, MH, 0, 0

    st7796_write_cmd(0x3A); // Pixel Format Set
    st7796_write_data_byte(0x55); // 16 bits/pixel

    st7796_write_cmd(0x29); // Display ON
    sleep_ms(20);
}

void st7796_set_window(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end) {
    st7796_write_cmd(0x2A); // Column Address Set
    gpio_put(PIN_DISP_CS, 0);
    gpio_put(PIN_DISP_DC, 1);
    uint8_t x_data = {(x_start >> 8) & 0xFF, x_start & 0xFF, (x_end >> 8) & 0xFF, x_end & 0xFF};
    spi_write_blocking(SPI_PORT, x_data, 4);
    gpio_put(PIN_DISP_CS, 1);

    st7796_write_cmd(0x2B); // Row Address Set
    gpio_put(PIN_DISP_CS, 0);
    gpio_put(PIN_DISP_DC, 1);
    uint8_t y_data = {(y_start >> 8) & 0xFF, y_start & 0xFF, (y_end >> 8) & 0xFF, y_end & 0xFF};
    spi_write_blocking(SPI_PORT, y_data, 4);
    gpio_put(PIN_DISP_CS, 1);

    st7796_write_cmd(0x2C); // Memory Write
}

void st7796_write_data_dma(const uint8_t *data, size_t len) {
    while (dma_busy) tight_loop_contents(); // Espera a transferência anterior
    dma_busy = true;
    gpio_put(PIN_DISP_CS, 0);
    gpio_put(PIN_DISP_DC, 1);
    dma_channel_transfer_from_buffer_now(dma_chan, data, len);
}

bool st7796_is_dma_busy(void) {
    return dma_busy;
}