#ifndef UI_MANAGER_H
#define UI_MANAGER_H

/**
 * @brief Inicializa todas as telas e widgets da UI.
 * As telas são criadas mas permanecem ocultas até serem ativadas.
 */
void ui_init();

/**
 * @brief Exibe a tela de boas-vindas.
 */
void ui_show_welcome_screen();

/**
 * @brief Exibe a tela indicando que o QR code está sendo lido.
 */
void ui_show_scanning_qr_screen();

/**
 * @brief Exibe a tela solicitando que o usuário coloque o dedo no sensor.
 */
void ui_show_place_finger_screen();

/**
 * @brief Exibe a tela indicando que a digital está sendo lida.
 */
void ui_show_scanning_fp_screen();

/**
 * @brief Exibe a tela de sucesso do check-in.
 */
void ui_show_success_screen();

/**
 * @brief Exibe uma tela de erro com uma mensagem específica.
 * 
 * @param message A mensagem de erro a ser exibida.
 */
void ui_show_error_screen(const char *message);

#endif // UI_MANAGER_H