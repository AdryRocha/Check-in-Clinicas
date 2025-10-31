// src/include/ui/lv_conf.h
#ifndef LV_CONF_H
#define LV_CONF_H

/* Configurações de resolução do display */
#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 1
#define LV_HOR_RES_MAX (320)    // CORRIGIDO: 320 pixels
#define LV_VER_RES_MAX (480)    // CORRIGIDO: 480 pixels

/* Fontes padrão */
#define LV_FONT_DEFAULT &lv_font_montserrat_16

/* Habilita as fontes que vamos usar no projeto */
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_22 1
#define LV_FONT_MONTSERRAT_24 1

/* Configurações de memória */
#define LV_MEM_CUSTOM 0
#define LV_MEM_SIZE (48 * 1024)  // 48KB para LVGL

/* Configurações de desempenho */
#define LV_DISP_DEF_REFR_PERIOD 30  // Atualiza a cada 30ms
#define LV_INDEV_DEF_READ_PERIOD 30
#define LV_USE_PERF_MONITOR 0

/* Configurações de log */
#define LV_USE_LOG 1
#if LV_USE_LOG
    #define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
    #define LV_LOG_PRINTF 1
#endif

/* Configurações de animação */
#define LV_USE_ANIMATION 1

/* Configurações de temas */
#define LV_USE_THEME_DEFAULT 1
#define LV_THEME_DEFAULT_DARK 0

/* Widgets habilitados */
#define LV_USE_ARC 1
#define LV_USE_BAR 1
#define LV_USE_BTN 1
#define LV_USE_BTNMATRIX 1
#define LV_USE_CANVAS 0
#define LV_USE_CHECKBOX 1
#define LV_USE_DROPDOWN 1
#define LV_USE_IMG 1
#define LV_USE_LABEL 1
#define LV_USE_LINE 1
#define LV_USE_LIST 1
#define LV_USE_MENU 0
#define LV_USE_MSGBOX 1
#define LV_USE_ROLLER 1
#define LV_USE_SLIDER 1
#define LV_USE_SPAN 0
#define LV_USE_SPINBOX 0
#define LV_USE_SPINNER 1
#define LV_USE_SWITCH 1
#define LV_USE_TEXTAREA 1
#define LV_USE_TABLE 0
#define LV_USE_TABVIEW 0
#define LV_USE_TILEVIEW 0
#define LV_USE_WIN 0

/* Layouts */
#define LV_USE_FLEX 1
#define LV_USE_GRID 1

/* Outras configurações */
#define LV_USE_ASSERT_NULL 1
#define LV_USE_ASSERT_MALLOC 1
#define LV_USE_ASSERT_STYLE 0
#define LV_USE_USER_DATA 1

#endif /*LV_CONF_H*/