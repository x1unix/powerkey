#ifndef _H_UTILS
#define _H_UTILS
#include <Arduino.h>

// Password data offset in EEPROM.
//
// Password struct memory layout looks like this:
//
//  [magic:2 bytes][len:2 bytes][data...]
#define EEPROM_PASSWD_OFFSET 0

#define EEPROM_MAGIC 0xDEAD
#define EEPROM_MAGIC_OFFSET EEPROM_PASSWD_OFFSET
#define EEPROM_PASSWD_LEN_OFFSET (EEPROM_MAGIC_OFFSET + sizeof(uint16_t))
#define EEPROM_PASSWD_DATA_OFFSET (EEPROM_PASSWD_LEN_OFFSET + sizeof(uint16_t))

bool isBoardDisabled();
void setupPins();
uint16_t getPasswdLen();
void readPasswdStr(uint16_t len, char* dst);
void savePasswd(uint16_t len, char* data);
void wipePasswd();

#endif
