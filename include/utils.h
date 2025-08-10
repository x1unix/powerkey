#ifndef _H_MASTERKEY
#define _H_MASTERKEY
#include <Arduino.h>

// Password length offset address in EEPROM.
#define EEPROM_PASSWD_LEN_OFFSET 0

// Password data offset in EEPROM.
//
// Password comes right after length value.
#define EEPROM_PASSWD_DATA_OFFSET sizeof(uint16_t)

enum BoardState {
  DISABLED = 0,
  PASSWD_EMPTY = 1
};

void boostrap();
bool isBoardDisabled();

#endif
