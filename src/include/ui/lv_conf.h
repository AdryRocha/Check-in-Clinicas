#ifndef LV_CONF_H
#define LV_CONF_H

/* Habilita as fontes que vamos usar no projeto */
#define LV_USE_FONT_MONTSERRAT 1
#define LV_FONT_MONTSERRAT_16 1 // <-- HABILITA A FONTE DE TAMANHO 16
#define LV_FONT_MONTSERRAT_20 1 // <-- HABILITA A FONTE DE TAMANHO 20

// Configurações essenciais do LVGL para este projeto
#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 1
#define LV_HOR_RES_MAX (160)
#define LV_VER_RES_MAX (240)
#define LV_FONT_DEFAULT &lv_font_montserrat_16

#endif /*LV_CONF_H*/