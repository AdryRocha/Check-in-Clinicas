#ifndef HAL_SPI_H
#define HAL_SPI_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Estrutura que define uma interface de comunicação SPI genérica.
 *
 * Esta estrutura utiliza ponteiros de função para abstrair as operações de baixo nível
 * do SPI, permitindo que os drivers de periféricos sejam independentes do microcontrolador.
 */
typedef struct {
    /**
     * @brief Ponteiro para a função que inicializa o barramento SPI.
     */
    void (*init)(void);

    /**
     * @brief Ponteiro para a função que escreve um bloco de dados no barramento SPI.
     * @param data Ponteiro para o buffer de dados a serem enviados.
     * @param len Número de bytes a serem enviados.
     */
    void (*write)(const uint8_t* data, size_t len);

    /**
     * @brief Ponteiro para a função que lê um bloco de dados do barramento SPI.
     * @param buffer Ponteiro para o buffer onde os dados lidos serão armazenados.
     * @param len Número de bytes a serem lidos.
     */
    void (*read)(uint8_t* buffer, size_t len);

    /**
     * @brief Ponteiro para a função que controla o pino de Chip Select (CS).
     * @param active Se true, ativa o CS (geralmente nível baixo). Se false, desativa (nível alto).
     */
    void (*set_cs)(bool active);

    /**
     * @brief Ponteiro para a função que controla o pino de Data/Command (DC).
     *        (Relevante apenas para periféricos como displays).
     * @param is_data Se true, define o pino para o modo de dados. Se false, para o modo de comando.
     */
    void (*set_dc)(bool is_data);

    /**
     * @brief Ponteiro para a função que controla o pino de Reset (RST).
     *        (Relevante para periféricos que necessitam de um reset de hardware).
     * @param active Se true, ativa o reset (geralmente nível baixo). Se false, desativa.
     */
    void (*set_rst)(bool active);

} hal_spi_interface_t;

#endif // HAL_SPI_H