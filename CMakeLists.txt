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

set(PICO_BOARD pico_w CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.14)

include(pico_sdk_import.cmake)

if(POLICY CMP0177)
  cmake_policy(SET CMP0177 NEW)
endif()

project(clinic_checkin C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# === LVGL ===
include(FetchContent)
FetchContent_Declare(
    lvgl
    GIT_REPOSITORY https://github.com/lvgl/lvgl.git
    GIT_TAG v8.3.11
)
FetchContent_MakeAvailable(lvgl)

target_include_directories(lvgl PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ui)
target_compile_definitions(lvgl INTERFACE LV_CONF_INCLUDE_SIMPLE)

# === FatFs ===
add_subdirectory(lib/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI)

# === Aplicação ===
# Adiciona todas as pastas de 'src' ao caminho de inclusão
include_directories(src)
    #src/include
    #src/core
    #src/drivers
    #src/services
    #src/ui
    #src/utils
    #src/hal


add_executable(clinic_checkin
    src/main.c
    src/system_init.c
    src/core/state_machine.c
    src/core/event_handler.c
    src/drivers/ST7796_display.c
    src/drivers/gm67_qr.c
    src/drivers/r307s_fingerprint.c
    src/drivers/sdcard_spi.c
    src/services/data_storage.c
    src/services/authentication.c
    src/services/network_sync.c
    src/ui/lvgl_port.c
    src/ui/ui_manager.c
    src/utils/error_handler.c
    src/hal/hal_spi_rp2040.c
)

target_link_libraries(clinic_checkin PRIVATE
    pico_stdlib
    pico_multicore
    hardware_spi
    hardware_uart
    hardware_dma
    hardware_gpio
    lvgl
    FatFs_SPI
)

pico_enable_stdio_usb(clinic_checkin 1)
pico_enable_stdio_uart(clinic_checkin 0)

pico_add_extra_outputs(clinic_checkin)