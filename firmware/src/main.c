
#include "debugUART.h"
#include "gpio.h"
#include "led.h"
#include "logo.h"
#include "eeprom.h" 
#include "sysctl.h" 
#include "rhsp.h"

//FIX: Make the version define come from cmake
#define VERSION "0.0.0"

//FIX: Make the LED pwmed so it isn't so bright

typedef struct {
    const char * name;
    int(* initializer)(void);
} Initializable;

const Initializable initalizables[] = {
    {
        "Status LED",
        led_init
    },
    {
        "eeprom",
        eeprom_init
    },
    // {
    //     "Upstream UART",
    //     rhsp_init
    // }
};

const int initalizableCount = sizeof(initalizables) / sizeof(Initializable);

uint8_t rhspAddress = 0;

int main(void) {
    gpio_enableAHB();

    if(debugUART_init() != 0) {
       goto cleanup;
    }

    debugUART_printString(logo);

    //ADD: Github link
    debugUART_printString("\n\nDuckLynx Version ");
    debugUART_printString(VERSION);
    //ADD: GPL information
    debugUART_printString("\nCopyright (C) 2025 Bryn \"bakl\" Hakl & Duck Tape and a Prayer\n");
    
    //ADD: Boot menu timeout
    debugUART_printString("\nPress any key enter boot menu (2)");
    debugUART_printString("\n\nStarting ...\n");

    for(uint32_t i = 0; i < initalizableCount; i++) {
        debugUART_printChar(':');
        int result = initalizables[i].initializer();
        if(result == 0) {
            debugUART_printString(") ");
        } else {
            debugUART_printString("( ");
        }

        debugUART_printStringConst(initalizables[i].name);
        debugUART_printChar('\n');
    }
    debugUART_printString("\nModule address: ");
    rhspAddress = eeprom_getAddress();
    debugUART_printWord(rhspAddress);

    while(1) {
        led_setColor(2, 2, 2);
        // rhsp_printSomething();
        sysctl_delay(4000000);

        led_setColor(0, 0, 0);
        sysctl_delay(4000000);
    }

    cleanup:
    while(1) {

    }
}
