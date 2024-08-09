# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Set name of project (as PROJECT_NAME) and C/C   standards
project(blink C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

# Include External Libs
set(PICO_PIO_USB_PATH ${CMAKE_SOURCE_DIR}/include/Pico-PIO-USB)
add_subdirectory(${PICO_PIO_USB_PATH} pico_pio_usb)

# Tell CMake where to find the executable source file
add_executable(${PROJECT_NAME} 
    debug.c
    main.c
    piuio.c
    usb_descriptors.c
    
    ${PICO_TINYUSB_PATH}/src/portable/raspberrypi/pio_usb/dcd_pio_usb.c
    ${PICO_TINYUSB_PATH}/src/portable/raspberrypi/pio_usb/hcd_pio_usb.c
)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    pico_pio_usb
    tinyusb_host
    pico_multicore
    tinyusb_device
)

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

# use tinyusb implementation
#target_compile_definitions(${PROJECT_NAME} PRIVATE PIO_USB_USE_TINYUSB)

# needed so tinyusb can find tusb_config.h
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})

# add flash target (make flash)
add_custom_target(flash DEPENDS ${PROJECT_NAME}.elf)
add_custom_command(TARGET flash
    USES_TERMINAL
    COMMAND sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "program ${PROJECT_NAME}.elf verify reset exit")
add_custom_target(pflash DEPENDS ${PROJECT_NAME}.uf2)
    add_custom_command(TARGET pflash
        USES_TERMINAL
        COMMAND picotool load ${PROJECT_NAME}.uf2 -f && picotool reboot)