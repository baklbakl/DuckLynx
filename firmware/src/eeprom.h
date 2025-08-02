#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>
int8_t eeprom_init(void);
int8_t eeprom_dump(void);
uint8_t eeprom_getAddress(void);

#endif
