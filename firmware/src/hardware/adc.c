#include "adc.h"
#include "debugUART.h"
#include "gpio.h"
#include "hardware/led.h"
#include "other.h"
#include "rhsp/rhsp.h"
#include "sysctl.h"
#include "register.h"

const uint8_t REGISTER_SYSCTL_PERIPHCTL_ADC_OFFSET = 0x38;
const uint32_t REGISTER_SYSCTL_PERIPHCTL_ADC_MODULE_0_INSTANCEMASK = 0b1 << 0;
const uint32_t REGISTER_SYSCTL_PERIPHCTL_ADC_MODULE_1_INSTANCEMASK = 0b1 << 1;

const uint32_t REGISTER_SYSCTL_PERIPHCTL_GPIO_PORT_P_INSTANCEMASK = 0b1 << 13;
const uint32_t REGISTER_SYSCTL_PERIPHCTL_GPIO_PORT_D_INSTANCEMASK = 0b1 << 3;

const uint32_t REGISTER_GPIO_D_BASE = REGISTER_GPIO_BASE + 0x57000;
const uint32_t REGISTER_GPIO_P_BASE = REGISTER_GPIO_BASE + 0x61000;

const uint32_t REGISTER_ADC_BASE = 0x40038000;

const uint32_t REGISTER_ADC_ADCACTSS_OFFSET = 0x0;
const uint32_t REGISTER_ADC_ADCACTSS_ASEN3 = 0b1 << 3;
const uint32_t REGISTER_ADC_ADCACTSS_BUSY = 0b1 << 16;

const uint32_t REGISTER_ADC_ADCPSSI_OFFSET = 0x28;
const uint32_t REGISTER_ADC_ADCPSSI_SS3 = 0b1 << 3;

const uint32_t REGISTER_ADC_ADCCTL_OFFSET = 0x38;
const uint32_t REGISTER_ADC_ADCCTL_VREF = 0b1;
const uint32_t REGISTER_ADC_ADCCTL_DITHER = 0b1 << 6;

const uint32_t REGISTER_ADC_ADCSSMUX3_OFFSET = 0x0A0;
const uint32_t REGISTER_ADC_ADCSSMUXn_MUX0_MASK = 0b1111;
const uint32_t REGISTER_ADC_ADCSSMUXn_MUX0_SHIFT = 0;
const uint32_t REGISTER_ADC_ADCSSMUXn_MUX0_AIN_6 = 6;
const uint32_t REGISTER_ADC_ADCSSMUXn_MUX0_AIN_7 = 7;

const uint32_t REGISTER_ADC_ADCSSCTL3_OFFSET = 0x0A4;
const uint32_t REGISTER_ADC_ADCSSCTLn_D0 = 0b1 << 0;
const uint32_t REGISTER_ADC_ADCSSCTLn_END0 = 0b1 << 1;
const uint32_t REGISTER_ADC_ADCSSCTLn_IE0 = 0b1 << 2;
const uint32_t REGISTER_ADC_ADCSSCTLn_TS0 = 0b1 << 3;

const uint32_t REGISTER_ADC_ADCSSFIFO3_OFFSET = 0xA8;
const uint32_t REGISTER_ADC_ADCSSFIFOn_MASK = 0xFFF;

const uint32_t REGISTER_ADC_ADCSSFSTAT3_OFFSET = 0xAC;
const uint32_t REGISTER_ADC_ADCSSFSTAT3_EMPTY = 0b1 << 8;

const uint32_t REGISTER_ADC_ADCSSEMUX3_OFFSET = 0xB8;
const uint32_t REGISTER_ADC_ADCSSEMUXn_EMUX0 = 0b1;

const uint32_t REGISTER_ADC_0_BASE = REGISTER_ADC_BASE;
volatile uint32_t * const REGISTER_ADC_0_ADCACTSS = (uint32_t *)(REGISTER_ADC_0_BASE + REGISTER_ADC_ADCACTSS_OFFSET);
volatile uint32_t * const REGISTER_ADC_0_ADCPSSI = (uint32_t *)(REGISTER_ADC_0_BASE + REGISTER_ADC_ADCPSSI_OFFSET);
volatile uint32_t * const REGISTER_ADC_0_ADCCT = (uint32_t *)(REGISTER_ADC_0_BASE + REGISTER_ADC_ADCCTL_OFFSET);
volatile uint32_t * const REGISTER_ADC_0_ADCSSMUX3 = (uint32_t *)(REGISTER_ADC_0_BASE + REGISTER_ADC_ADCSSMUX3_OFFSET);
volatile uint32_t * const REGISTER_ADC_0_ADCSSCTL3 = (uint32_t *)(REGISTER_ADC_0_BASE + REGISTER_ADC_ADCSSCTL3_OFFSET);
volatile uint32_t * const REGISTER_ADC_0_ADCSSFIFO3 = (uint32_t *)(REGISTER_ADC_0_BASE + REGISTER_ADC_ADCSSFIFO3_OFFSET);
volatile uint32_t * const REGISTER_ADC_0_ADCSSFSTAT3 = (uint32_t *)(REGISTER_ADC_0_BASE + REGISTER_ADC_ADCSSFSTAT3_OFFSET);
volatile uint32_t * const REGISTER_ADC_0_ADCSSEMUX3 = (uint32_t *)(REGISTER_ADC_0_BASE + REGISTER_ADC_ADCSSEMUX3_OFFSET);

//VREFA+ is 2.994 volts
int8_t adc_init() {
    sysctl_enablePeripheral(REGISTER_SYSCTL_PERIPHCTL_ADC_OFFSET, REGISTER_SYSCTL_PERIPHCTL_ADC_MODULE_0_INSTANCEMASK);
    sysctl_enablePeripheral(REGISTER_SYSCTL_PERIPHCTL_GPIO_OFFSET, REGISTER_SYSCTL_PERIPHCTL_GPIO_PORT_D_INSTANCEMASK | REGISTER_SYSCTL_PERIPHCTL_GPIO_PORT_P_INSTANCEMASK);
     
    //Switch the pins over to analog mode
    //5V
    gpio_enableAnalog(REGISTER_GPIO_D_BASE, REGISTER_GPIO_PIN_2);
    //Battery current and voltage
    gpio_enableAnalog(REGISTER_GPIO_P_BASE, REGISTER_GPIO_PIN_0 | REGISTER_GPIO_PIN_1);


    //Disable the ADC sequencer to prevent unintentional sampling
    *REGISTER_ADC_0_ADCACTSS &= ~REGISTER_ADC_ADCACTSS_ASEN3;

    //Enable using VREFA and dithering
    *REGISTER_ADC_0_ADCCT |= REGISTER_ADC_ADCCTL_VREF | REGISTER_ADC_ADCCTL_DITHER;

    //ADD: Hardware averaging to ADC

    //Mark this as the (first and) last sample in the sequence
    *REGISTER_ADC_0_ADCSSCTL3 = (*REGISTER_ADC_0_ADCSSCTL3 & (~REGISTER_ADC_ADCSSCTLn_D0) & (~REGISTER_ADC_ADCSSCTLn_TS0) & (~REGISTER_ADC_ADCSSCTLn_IE0)) | REGISTER_ADC_ADCSSCTLn_END0;

    adc_checkBatteryVoltage(*timer_value);
    return 0;
}

uint16_t internalRead() {
    //Reenable the sequencer
    *REGISTER_ADC_0_ADCACTSS |= REGISTER_ADC_ADCACTSS_ASEN3;

     //Start the sequencer
    *REGISTER_ADC_0_ADCPSSI |= REGISTER_ADC_ADCPSSI_SS3;

    //Wait for sampling to finish
    while(*REGISTER_ADC_0_ADCACTSS & REGISTER_ADC_ADCACTSS_BUSY) {}

    uint16_t value = UINT16_MAX;
    if(!(*REGISTER_ADC_0_ADCSSFSTAT3 & REGISTER_ADC_ADCSSFSTAT3_EMPTY)) {
        value = *REGISTER_ADC_0_ADCSSFIFO3 & REGISTER_ADC_ADCSSFIFOn_MASK;
    }

    //Make sure to empty the FIFO if it isn't already for the next sequencer run
    while(!(*REGISTER_ADC_0_ADCSSFSTAT3 & REGISTER_ADC_ADCSSFSTAT3_EMPTY)) {
        *REGISTER_ADC_0_ADCSSFIFO3;
    }

    //Disable sequencer. Possibly unneeded. Could save time by disabling this
    *REGISTER_ADC_0_ADCACTSS &= ~REGISTER_ADC_ADCACTSS_ASEN3;

    return value;
}

//Reads the raw value of an analog channel. Returns UINT16_MAX on failure
uint16_t adc_readRaw(uint8_t ain) {
    //Only AIN0 - AIN23. No need to check for less that zero. ain is unsigned
    if(ain > 23) {
        return UINT16_MAX;
    }
    
    //Select the input. The MSB for pin selection lives in another register for some reason
    *REGISTER_ADC_0_ADCSSMUX3 = (*REGISTER_ADC_0_ADCSSMUX3 & ~REGISTER_ADC_ADCSSMUXn_MUX0_MASK) | ((REGISTER_ADC_ADCSSMUXn_MUX0_MASK & ain) << REGISTER_ADC_ADCSSMUXn_MUX0_SHIFT);
    *REGISTER_ADC_0_ADCSSEMUX3 = (*REGISTER_ADC_0_ADCSSEMUX3 & ~REGISTER_ADC_ADCSSEMUXn_EMUX0) | (ain / (REGISTER_ADC_ADCSSMUXn_MUX0_MASK + 1));

    return internalRead();
}

//Result the raw value from the analog channel
uint16_t adc_readTemp() {
    //No need to mask away all the other bits as nothing should set them.
    *REGISTER_ADC_0_ADCSSCTL3 |= REGISTER_ADC_ADCSSCTLn_END0 | REGISTER_ADC_ADCSSCTLn_TS0;
    uint16_t value = internalRead();
    
    *REGISTER_ADC_0_ADCSSCTL3 = (*REGISTER_ADC_0_ADCSSCTL3 & (~REGISTER_ADC_ADCSSCTLn_TS0)) | REGISTER_ADC_ADCSSCTLn_END0;
    return value;
}

int8_t adc_readMapped(ADC_CHANNEL channel, int16_t * outValue, uint8_t raw) {
    // Since the ADC result is out of 4096 (12 bit) all readings are divided by this value first to convert to precent. 
    // This is then multiplied by the VREFA+ voltage of 2.994 to find the actual voltage. The VREFA- pin is tied to GND,
    // so it doesn't need to be accounted for. From there it is converted to whatever unit is needed and the specific
    // measurement. This yields a formula of V = Counts / ADC max value * VREFA+ or with the constants
    // V = Counts / 4096 * 2.994. All the numbers are then condensed together to make a multiply value, a divide value, 
    // and an add value. These are applied in such a way to preserve as much precision as possible while avoiding floating
    // point numbers.

    // For voltage measurements:
    // The unit is millivolts so the voltage is multiplied by 1000. Then whatever divider was used is also multiplied in.
    // This yields a formula of Vout = Vmeasured * 1000 * (1 / Voltage Divider)

    uint16_t value = 0; 
    uint32_t multiply = 1;
    int32_t divide = 1;
    int16_t add = 0;

    switch (channel) {
        case ADC_CHANNEL_BATTERY_CURRENT:
            // According to the datasheet of the high side current monitor, The output voltage is equal to 0.01 * Rout * Vsense.
            // Rout is 3000 ohms for the battery. Vsense is equal to I * R where R is the resistance of the shunt resistor. 
            // When combined that yields 0.01 * 3000 * I * 0.003 = Vout. When rearranged to be in terms of Vout that equals
            // Vout / 0.09 = I and when converted to miliamps and rearranged it yields Vout * 100000 / 9 = I
            multiply = 299400;
            divide = 36864;
            value = adc_readRaw(13);
            break;

        case ADC_CHANNEL_FIVE_VOLTS:
            // Uses a 1/2 voltage divider.
            multiply = 5988;
            divide = 4096;
            value = adc_readRaw(23);
            break;

        case ADC_CHANNEL_BATTERY_VOLTS:
            // Uses 1/6 voltage divider.

            multiply = 17964;
            divide = 4096;
            value = adc_readRaw(22);
            break;
        
        case ADC_CHANNEL_CONTROLLER_TEMPERATURE:
            //Magic conversion from raw counts to degrees C. See datasheet page 837. 
            // Equation Vtsense = 2.7 - ((TEMP + 55) / 75) and Vtsense = RAW_COUNTS / 4096 * 2.994. 
            // When put togeher and put in terms of RAW_COUNTS it comes to 75(2.7-(RAW_COUNTS / 4096 * 2.994))-55
            // When moved around into a way to preserve as much precision as possible, avoid floating point,
            // and convert to deci-degC it comes out to this equation
            // return 1475 - ((2246U * internalRead()) / 4096);

            multiply = 2246;
            divide = -4096;
            add = 1475;
            value = adc_readTemp();
            break;            

        case ADC_CHANNEL_USER_0:
        case ADC_CHANNEL_USER_1:
        case ADC_CHANNEL_USER_2:
        case ADC_CHANNEL_USER_3:

        case ADC_CHANNEL_GPIO_CURRENT:
        case ADC_CHANNEL_I2C_CURRENT:
        case ADC_CHANNEL_SERVO_CURRENT:

        case ADC_CHANNEL_MOTOR_0_CURRENT:
        case ADC_CHANNEL_MOTOR_1_CURRENT:
        case ADC_CHANNEL_MOTOR_2_CURRENT:
        case ADC_CHANNEL_MOTOR_3_CURRENT:
            return 1;
            break;
    }

    
    if(raw) {
        //This cast could make weird errors if the top bit of value is set. The top bit never *should* be set because value is set by a 12 bit ADC read
        *outValue = value;
    } else {
        *outValue = (((int32_t) (multiply * value)) / divide) + add;
    }
    return 0;
}

uint32_t adc_lastBatteryVoltageReadTime = UINT32_MAX;

void adc_checkBatteryVoltage(uint32_t currentTime) {
    adc_lastBatteryVoltageReadTime = currentTime;
    int16_t outValue = 0;
    if(!adc_readMapped(ADC_CHANNEL_BATTERY_VOLTS, &outValue, 0)) {
        //Problem!!!
    }

    debugUART_printString("Battery voltage: ");
    debugUART_printWord(outValue);
    debugUART_printChar('\n');

    if(led_mode == LED_MODE_NO_BATTERY && outValue > ADC_LOW_BATTERY_CUTOFF_VOLTAGE) {
        debugUART_printString("Battery restored\n");
        //TEST: Does the stock firmware restore the step in the pattern after power is restored?
        led_setupPatternWithTime(led_oldMode, led_oldPattern, currentTime);

    } else if(led_mode != LED_MODE_NO_BATTERY && outValue <= ADC_LOW_BATTERY_CUTOFF_VOLTAGE) {
        rhsp_moduleStatus |= RHSP_MODULE_STATUS_BATTERY_LOW;
        //TEST: Does low power trigger a full failsafe or does it just diable motors on stock firmware?
        failsafe();

        led_oldMode = led_mode;
        led_mode = LED_MODE_NO_BATTERY;
        led_oldPattern = led_currentPattern;
        led_currentPattern = &led_noBatteryPattern;
        led_lastStepTime = currentTime;

        led_currentPatternStep = 0;
        led_PatternStep step = led_currentPattern->steps[led_currentPatternStep];
        led_setColor(step.red, step.green, step.blue);   
    }
}
