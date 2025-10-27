#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

#include "hardware/spi.h"
#include "hardware/uart.h"

// --- Configuração SPI (Display e Cartão SD) ---
#define SPI_PORT        spi0
#define PIN_SPI_SCK     18
#define PIN_SPI_TX      19
#define PIN_SPI_RX      16

// --- Configuração do Display ST7796 ---
#define PIN_DISP_CS     17
#define PIN_DISP_DC     20
#define PIN_DISP_RST    21
#define PIN_DISP_BLK    28
#define DISP_WIDTH      320
#define DISP_HEIGHT     480

// --- Configuração do Cartão SD ---
#define PIN_SD_CS       22

// --- Configuração UART (Leitor QR) ---
#define UART_QR_ID          uart0
#define UART_QR_BAUD_RATE   9600
#define PIN_QR_TX           0
#define PIN_QR_RX           1

// --- Configuração UART (Sensor de Impressão Digital) ---
#define UART_FP_ID          uart1
#define UART_FP_BAUD_RATE   57600
#define PIN_FP_TX           4
#define PIN_FP_RX           5

#endif // HARDWARE_CONFIG_H