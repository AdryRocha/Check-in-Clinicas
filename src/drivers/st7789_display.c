#include "st7789_display.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"

// Canal DMA a ser utilizado
static int dma_channel;

// Função auxiliar para enviar um comando ao display
static void st7789_send_cmd(uint8_t cmd) {
    gpio_put(DISPLAY_DC, 0); // Modo de comando
    gpio_put(DISPLAY_CS, 0);
    spi_write_blocking(DISPLAY_SPI, &cmd, 1);
    gpio_put(DISPLAY_CS, 1);
}

// Função auxiliar para enviar dados ao display
static void st7789_send_data(const uint8_t *data, size_t len) {
    gpio_put(DISPLAY_DC, 1); // Modo de dados
    gpio_put(DISPLAY_CS, 0);
    spi_write_blocking(DISPLAY_SPI, data, len);
    gpio_put(DISPLAY_CS, 1);
}

// Função auxiliar para definir a janela de desenho no display
static void st7789_set_window(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    uint8_t data;

    // Column Address Set
    st7789_send_cmd(0x2A);
    data = (x >> 8) & 0xFF;
    data = x & 0xFF;
    data = ((x + w - 1) >> 8) & 0xFF;
    data = (x + w - 1) & 0xFF;
    st7789_send_data(data, 4);

    // Row Address Set
    st7789_send_cmd(0x2B);
    data = (y >> 8) & 0xFF;
    data = y & 0xFF;
    data = ((y + h - 1) >> 8) & 0xFF;
    data = (y + h - 1) & 0xFF;
    st7789_send_data(data, 4);

    // Memory Write
    st7789_send_cmd(0x2C);
}

void st7789_init() {
    // Configuração do SPI
    spi_init(DISPLAY_SPI, 8 * 1000 * 1000); // 8 MHz
    gpio_set_function(DISPLAY_SCK, GPIO_FUNC_SPI);
    gpio_set_function(DISPLAY_MOSI, GPIO_FUNC_SPI);

    // Configuração dos pinos de controle
    gpio_init(DISPLAY_CS);
    gpio_set_dir(DISPLAY_CS, GPIO_OUT);
    gpio_put(DISPLAY_CS, 1);

    gpio_init(DISPLAY_DC);
    gpio_set_dir(DISPLAY_DC, GPIO_OUT);

    gpio_init(DISPLAY_RST);
    gpio_set_dir(DISPLAY_RST, GPIO_OUT);

    // Reset do display
    gpio_put(DISPLAY_RST, 1);
    sleep_ms(100);
    gpio_put(DISPLAY_RST, 0);
    sleep_ms(100);
    gpio_put(DISPLAY_RST, 1);
    sleep_ms(100);

    // Sequência de inicialização do ST7789
    st7789_send_cmd(0x11); // Sleep Out
    sleep_ms(120);

    st7789_send_cmd(0x36); // Memory Data Access Control
    uint8_t madctl = 0x00;
    st7789_send_data(&madctl, 1);

    st7789_send_cmd(0x3A); // Interface Pixel Format
    uint8_t pixfmt = 0x55; // 16 bits/pixel
    st7789_send_data(&pixfmt, 1);
    
    st7789_send_cmd(0x29); // Display ON
    sleep_ms(100);

    // Configuração do DMA
    dma_channel = dma_claim_unused_channel(true);
}

void st7789_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    // Garante que a transferência DMA anterior terminou
    dma_channel_wait_for_finish_blocking(dma_channel);

    uint16_t width = lv_area_get_width(area);
    uint16_t height = lv_area_get_height(area);

    st7789_set_window(area->x1, area->y1, width, height);

    gpio_put(DISPLAY_DC, 1); // Modo de dados
    gpio_put(DISPLAY_CS, 0);

    // Configura e inicia a transferência DMA
    dma_channel_config c = dma_channel_get_default_config(dma_channel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_dreq(&c, spi_get_dreq(DISPLAY_SPI, true));
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);

    dma_channel_configure(
        dma_channel,
        &c,
        &spi_get_hw(DISPLAY_SPI)->dr, // Endereço de escrita (SPI data register)
        color_p,                     // Endereço de leitura (LVGL buffer)
        width * height,              // Número de transferências
        true                         // Inicia imediatamente
    );
    
    // Informa ao LVGL que o buffer está pronto para ser usado novamente
    // A espera pelo fim do DMA é feita no início da próxima chamada a esta função
    lv_disp_flush_ready(disp_drv);
}