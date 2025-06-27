#include "eeprom.h"

#include <stdint.h>

#include "debugUART.h"
#include "sysctl.h"
#include "register.h"

const uint8_t REGISTER_SYSCTL_PERIPHCTL_EEPROM_OFFSET = 0x58;
const uint32_t REGISTER_SYSCTL_PERIPHCTL_EEPROM_INSTANCEMASK = 0b1;
const uint32_t REGISTER_SYSCTL_PERIPHCTL_RESET_BASE = REGISTER_SYSCTL_BASE + 0x500;
volatile uint32_t * const REGISTER_SYSCTL_RESET_EEPROM = (uint32_t *)(REGISTER_SYSCTL_PERIPHCTL_RESET_BASE + REGISTER_SYSCTL_PERIPHCTL_EEPROM_OFFSET);

const uint32_t REGISTER_EEPROM_BASE = 0x400AF000;

volatile uint32_t * const REGISTER_EEPROM_EEDONE = (uint32_t *)(REGISTER_EEPROM_BASE + 0x18);
const uint32_t REGISTER_EEPROM_EEDONE_WORKING = 0b1;

volatile uint32_t * const REGISTER_EEPROM_EESUPP = (uint32_t *)(REGISTER_EEPROM_BASE + 0x1C);
const uint32_t REGISTER_EEPROM_EESUPP_ERETRY = 0b1 << 2;
const uint32_t REGISTER_EEPROM_EESUPP_PRETRY = 0b1 << 3;

int eeprom_init(void) {
    sysctl_enablePeripheral(REGISTER_SYSCTL_PERIPHCTL_EEPROM_OFFSET, REGISTER_SYSCTL_PERIPHCTL_EEPROM_INSTANCEMASK);

    // ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
    // while(!ROM_SysCtlPeripheralReady(SYSCTL_PERIPH_EEPROM0)) {
    // }

    //Wait for the EEPROM to finish working
    while (*REGISTER_EEPROM_EEDONE & REGISTER_EEPROM_EEDONE_WORKING) {}

    if(*REGISTER_EEPROM_EESUPP & REGISTER_EEPROM_EESUPP_ERETRY) {
        return 1;
    }
    
    if(*REGISTER_EEPROM_EESUPP & REGISTER_EEPROM_EESUPP_PRETRY) {
        return 2;
    }

    *REGISTER_SYSCTL_RESET_EEPROM |= REGISTER_SYSCTL_PERIPHCTL_EEPROM_INSTANCEMASK;
    *REGISTER_SYSCTL_RESET_EEPROM &= ~(REGISTER_SYSCTL_PERIPHCTL_EEPROM_INSTANCEMASK);

    sysctl_waitForReadyPeripheral(REGISTER_SYSCTL_PERIPHCTL_EEPROM_OFFSET, REGISTER_SYSCTL_PERIPHCTL_EEPROM_INSTANCEMASK);

    while (*REGISTER_EEPROM_EEDONE & REGISTER_EEPROM_EEDONE_WORKING) {}

    if(*REGISTER_EEPROM_EESUPP & REGISTER_EEPROM_EESUPP_ERETRY) {
        return 1;
    }
    
    if(*REGISTER_EEPROM_EESUPP & REGISTER_EEPROM_EESUPP_PRETRY) {
        return 2;
    }

    // return 0;

    // uint32_t eepromStatus = EEPROMInit();

    // if(eepromStatus == EEPROM_INIT_OK) {
        // return 0;
    // } 
    
    // if(eepromStatus == EEPROM_INIT_ERROR) {
        // return 1;
    // } 
    
    // return 2;

    return 0;
    
}

volatile uint32_t * const REGISTER_EEPROM_EEBLOCK = (uint32_t *)(REGISTER_EEPROM_BASE + 0x4);
const uint32_t REGISTER_EEPROM_EEBLOCK_MASK = 0xFFFF;

volatile uint32_t * const REGISTER_EEPROM_EEOFFSET = (uint32_t *)(REGISTER_EEPROM_BASE + 0x8);
const uint32_t REGISTER_EEPROM_EEOFFSET_MASK = 0b1111;

volatile uint32_t * const REGISTER_EEPROM_EERDWR = (uint32_t *)(REGISTER_EEPROM_BASE + 0x10);
volatile uint32_t * const REGISTER_EEPROM_EERDWRINC = (uint32_t *)(REGISTER_EEPROM_BASE + 0x14);

uint8_t eeprom_getAddress(void) {
    //Set the block and offset to zero
    *REGISTER_EEPROM_EEBLOCK &= ~REGISTER_EEPROM_EEBLOCK_MASK;
    *REGISTER_EEPROM_EEOFFSET &= ~REGISTER_EEPROM_EEOFFSET_MASK;
    return (*REGISTER_EEPROM_EERDWR);
}

int eeprom_dump(void) {
    debugUART_printString("Eeprom dump started\n");

    //Set the block and offset to zero
    *REGISTER_EEPROM_EEBLOCK &= ~REGISTER_EEPROM_EEBLOCK_MASK;
    *REGISTER_EEPROM_EEOFFSET &= ~REGISTER_EEPROM_EEOFFSET_MASK;
    
    const uint8_t eepromBlockCount = 32;
    const uint8_t eepromBlockWords = 16;

    //Loop through the eeprom
    for (uint8_t i = 0; i < eepromBlockCount; i++) {
        for(uint8_t offset = 0; offset < eepromBlockWords; offset++) {
            debugUART_printWordHex(*REGISTER_EEPROM_EERDWRINC);
        }
        (*REGISTER_EEPROM_EEBLOCK)++;
    }
    debugUART_printString("\nDump Finished\n");
    return 0;
}
