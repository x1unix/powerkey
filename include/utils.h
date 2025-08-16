#ifndef _H_UTILS
#define _H_UTILS
#include <Arduino.h>

// Password length offset address in EEPROM.
#define EEPROM_PASSWD_LEN_OFFSET 0

// Password data offset in EEPROM.
//
// Password comes right after length value.
#define EEPROM_PASSWD_DATA_OFFSET EEPROM_PASSWD_LEN_OFFSET + sizeof(uint16_t)

bool isBoardDisabled();
void setupPins();
uint16_t getPasswdLen();
void readPasswdStr(uint16_t len, char* dst);
void savePasswd(uint16_t len, char* data);

#endif
