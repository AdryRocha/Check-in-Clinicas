# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Versão mínima do CMake
cmake_minimum_required(VERSION 3.14)

# Importa as definições do Pico SDK antes da declaração do projeto
include(pico_sdk_import.cmake)

# Declaração do projeto
project(clinic_checkin C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Inicializa o Pico SDK
pico_sdk_init()

# --- Integração de Bibliotecas de Terceiros ---

# 1. LVGL (Light and Versatile Graphics Library)
include(FetchContent)
FetchContent_Declare(
    lvgl
    GIT_REPOSITORY https://github.com/lvgl/lvgl.git
    GIT_TAG v8.3.11 # Fixa uma versão estável para garantir a reprodutibilidade
)
FetchContent_MakeAvailable(lvgl)

# --- CORREÇÃO FINAL ---
# Adiciona o diretório correto do nosso lv_conf.h à compilação INTERNA do LVGL.
# O caminho agora reflete a sua estrutura de pastas.
target_include_directories(lvgl PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ui)
# Diz ao LVGL (e a qualquer alvo que o utilize) para usar #include "lv_conf.h"
target_compile_definitions(lvgl INTERFACE LV_CONF_INCLUDE_SIMPLE)
# --- FIM DA CORREÇÃO ---

# 2. FatFs (Filesystem for SD Card)
add_subdirectory(lib/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI)

# --- Agregação de Arquivos Fonte da Aplicação ---

# Adiciona o diretório 'include' principal ao caminho de busca de headers
include_directories(src/include)

# Lista todos os arquivos fonte da aplicação
add_executable(clinic_checkin
    src/main.c
    src/system_init.c
    src/core/state_machine.c
    src/core/event_handler.c
    src/drivers/ST7789_display.c
    src/drivers/gm67_qr.c
    src/drivers/r307s_fingerprint.c
    src/drivers/sdcard_spi.c
    src/services/data_storage.c
    src/services/authentication.c
    src/services/network_sync.c
    src/ui/lvgl_port.c
    src/ui/ui_manager.c
    src/utils/error_handler.c
)

# --- Vinculação de Bibliotecas (Linking) ---

# Vincula o executável principal com as bibliotecas do SDK e de terceiros
target_link_libraries(clinic_checkin PRIVATE
    pico_stdlib
    pico_multicore
    hardware_spi
    hardware_uart
    hardware_dma
    hardware_gpio
    lvgl              # Vincula com a biblioteca LVGL
    FatFs_SPI         # Vincula com a biblioteca FatFs para SPI
)

# --- Configurações Finais ---

# Habilita a saída de depuração via USB e desabilita a via UART
pico_enable_stdio_usb(clinic_checkin 1)
pico_enable_stdio_uart(clinic_checkin 0)

# Gera arquivos adicionais como.uf2,.hex,.bin
pico_add_extra_outputs(clinic_checkin)
