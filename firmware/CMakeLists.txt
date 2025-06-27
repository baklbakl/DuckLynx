cmake_minimum_required(VERSION 3.10)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(CMAKE_TOOLCHAIN_FILE armnative.cmake)

project(expansionHubFW LANGUAGES C)
set(CMAKE_C_STANDARD 23)
# set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE MinSizeRel)
# set(CMAKE_BUILD_TYPE Debug)

if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -D DEBUG")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -isystem /usr/lib/gcc/arm-none-eabi/14.2.0/include -isystem /usr/lib/gcc/arm-none-eabi/14.2.0/include-fixed -isystem /usr/arm-none-eabi/include/ -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -ffunction-sections -fdata-sections -nostartfiles -Wall -Wpedantic -Werror=return-type -DPART_TM4C123GH6PGE -DTARGET_IS_TM4C123_RB1 -Dgcc")

# Define a variable containing a list of source files for the project
set(SRC_FILES
        src/main.c
        src/startup.c
        src/debugUART.c
        src/led.c
        src/logo.c
        src/eeprom.c
        src/sysctl.c
        src/debug.c
        src/gpio.c
        src/rhsp.c
        src/rhspUART.c
        src/UART.c

        # src/driverlib/adc.c
        # # src/driverlib/aes.c
        # # src/driverlib/can.c
        # src/driverlib/comp.c
        # src/driverlib/cpu.c
        # # src/driverlib/crc.c
        # # src/driverlib/des.c
        # src/driverlib/eeprom.c
        # # src/driverlib/emac.c
        # src/driverlib/epi.c
        # src/driverlib/flash.c
        # src/driverlib/fpu.c
        # src/driverlib/gpio.c
        # src/driverlib/hibernate.c
        # src/driverlib/i2c.c
        # src/driverlib/interrupt.c
        # # src/driverlib/lcd.c
        # src/driverlib/mpu.c
        # src/driverlib/onewire.c
        # src/driverlib/pwm.c
        # src/driverlib/qei.c
        # # src/driverlib/shamd5.c
        # src/driverlib/ssi.c
        # # src/driverlib/sw_crc.c
        # src/driverlib/sysctl.c
        # src/driverlib/sysexc.c
        # src/driverlib/systick.c
        # src/driverlib/timer.c
        # src/driverlib/uart.c
        # src/driverlib/udma.c
        # src/driverlib/usb.c
        # src/driverlib/watchdog.c
)

# Define the build target for the executable
add_executable(${PROJECT_NAME}
        ${SRC_FILES})

# set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE "-T" "${CMAKE_SOURCE_DIR}/src/linkerconfig.ld" "-Xlinker" "--gc-sections")
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        #COMMAND avr-objcopy -j .text -j .data -O ihex ${PROJECT_NAME} ${PROJECT_NAME}.hex && avrdude -p m2560 -c stk500v2 -P /dev/ttyACM0 -b 115200 -F -D -U flash:w:${PROJECT_NAME}.hex && exit 0
        COMMAND arm-none-eabi-objcopy -O binary ${PROJECT_NAME} ${PROJECT_NAME}.bin && echo "uploading doesnt work yet"
        COMMENT "Uploading ..."
)
