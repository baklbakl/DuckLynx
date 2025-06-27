#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>
int eeprom_init(void);
int eeprom_dump(void);
uint8_t eeprom_getAddress(void);

#endif
