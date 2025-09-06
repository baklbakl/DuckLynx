#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>
int8_t eeprom_init(void);
int8_t eeprom_dump(void);
uint8_t eeprom_getAddress(void);
void eeprom_setAddress(uint8_t newAddress);

extern uint8_t rhspAddress;

#endif
