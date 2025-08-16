#include <Arduino.h>
#include <EEPROM.h>
#include "config.h"
#include "utils.h"

bool isBoardDisabled() {
  pinMode(PIN_GUARD, INPUT_PULLUP);
  int val = digitalRead(PIN_GUARD);
  return val == LOW;
}

void setupPins() {
  pinMode(PIN_LED_SETUP, OUTPUT);
  pinMode(PIN_LED_MASTER, OUTPUT);
  pinMode(PIN_BTN_SETUP_MODE, INPUT_PULLUP);
  pinMode(PIN_BTN_MASTER_KEY, INPUT_PULLUP);
}

uint16_t getPasswdLen() {
  uint16_t val = 0;
  EEPROM.get(EEPROM_PASSWD_LEN_OFFSET, val);
  return val;
}

void savePasswd(uint16_t len, char* data) {
  // Commit len
  EEPROM.write(EEPROM_PASSWD_LEN_OFFSET, len & 0xFF);
  EEPROM.write(EEPROM_PASSWD_LEN_OFFSET + 1, (len >> 8) & 0xFF);

  for (uint16_t i = 0; i < len; i++) {
    EEPROM.write(EEPROM_PASSWD_DATA_OFFSET + i, data[i]);
  }
}

void readPasswdStr(uint16_t len, char* dst) {
  if (len == 0) {
    return;
  }

  for (uint8_t i = 0; i < len; i++) {
    dst[i] = EEPROM.read(EEPROM_PASSWD_DATA_OFFSET + i);
  }
}
