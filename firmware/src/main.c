
#include "other.h"
#include "hardware/adc.h"
#include "debugUART.h"
#include "hardware/gpio.h"
#include "hardware/led.h"
#include "hardware/timer.h"
#include "logo.h"
#include "hardware/eeprom.h" 
#include "hardware/register.h" 
#include "rhsp/rhsp.h"
#include "rhsp/rhspUART.h"
#include "hardware/dma.h"
#include "hardware/motor.h"

//FIX: Make the version define come from cmake
#define VERSION "0.0.0"

typedef struct {
    const char * name;
    int8_t(* initializer)(void);
} Initializable;

const Initializable initalizables[] = {
    {
        "Timer",
        timer_init
    },
    {
        "Status LED",
        led_init
    },
    {
        "EEPROM",
        eeprom_init
    },
    {
        "ADC",
        adc_init
    },
    {
        "Motor",
        motor_init
    },
    {
        "RHSP Decoder",
        rhsp_init
    },
    {
        "DMA",
        dma_init
    },
    {
        "RHSP UART",
        rhspUART_init
    },
};

const int initalizableCount = sizeof(initalizables) / sizeof(Initializable);

int main(void) {
    gpio_enableAHB();

    if(debugUART_init() != 0) {
       goto cleanup;
    }

    debugUART_printStringConst(logo);
    debugUART_printString("\n\nDuckLynx Version ");
    debugUART_printString(VERSION);
    debugUART_printString("\nCopyright (C) 2025 Bryn \"bakl\" Hakl & Duck Tape and a Prayer\nThis software comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it under the conditions of the GNU Public Licence version 3 which can be found at https://www.gnu.org/licenses/gpl-3.0.en.html. The source code of this software can be found at https://github.com/baklbakl/DuckLynx/\n\nStarting ...\n");
    
    //ADD: Boot menu with self tests?
    // debugUART_printString("\nPress any key enter boot menu (2)");
    
    for(uint32_t i = 0; i < initalizableCount; i++) {
        debugUART_printChar(':');
        int8_t result = initalizables[i].initializer();
        if(result == 0) {
            debugUART_printString(") ");
        } else {
            debugUART_printString("( ");
        }

        debugUART_printStringConst(initalizables[i].name);
        debugUART_printChar('\n');

        if(result != 0) {
            goto cleanup;
        }
    }

    debugUART_printString("\nModule address: ");
    debugUART_printWord(rhspAddress);
    debugUART_printString("\n\n");

    // goto cleanup;

    while(1) {
        // debugUART_printString("Check: ");
        // debugUART_printU8Hex(rhsp_lastCommandTime - (*timer_value) > RHSP_TIMEOUT && rhsp_lastCommandTime != UINT32_MAX && !(rhsp_moduleStatus & RHSP_MODULE_STATUS_KEEP_ALIVE_TIMEOUT));
        // debugUART_printChar('\n');
        
        // debugUART_printString("Check: ");
        // debugUART_printWordHex(rhsp_lastCommandTime);
        // debugUART_printChar('\n');
        uint32_t lastCommandTime_cached = rhsp_lastCommandTime;
        uint32_t timerValue = *timer_value;
        if(lastCommandTime_cached - timerValue >= RHSP_TIMEOUT && lastCommandTime_cached != UINT32_MAX && led_mode != LED_MODE_TIMEOUT && led_mode != LED_MODE_NO_BATTERY) {
            rhsp_moduleStatus |= RHSP_MODULE_STATUS_KEEP_ALIVE_TIMEOUT;
            
            led_setupPatternWithTime(LED_MODE_TIMEOUT, &led_timeoutPattern, timerValue);
            failsafe();
        }

        //Step through led pattern steps if needed.
        if(led_currentPattern->totalSteps > 1 && led_lastStepTime - timerValue >= (led_currentPattern->steps[led_currentPatternStep].tenths * TIMER_TICKS_PER_100MS)) {
            led_lastStepTime = timerValue;
            led_currentPatternStep++;
            if(led_currentPatternStep >= led_currentPattern->totalSteps) {
                led_currentPatternStep = 0;
            }
            
            led_PatternStep step = led_currentPattern->steps[led_currentPatternStep];
            led_setColor(step.red, step.green, step.blue);    
        }

        //This get initalized by adc_init()
        if(adc_lastBatteryVoltageReadTime - timerValue >= ADC_BATTERY_VOLTAGE_READ_INTERVAL) {
            adc_checkBatteryVoltage(timerValue);
        }
    }

    cleanup:
    while(1) {

    }
}
