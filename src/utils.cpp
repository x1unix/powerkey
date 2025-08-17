#include <Arduino.h>
#include <EEPROM.h>
#include "config.h"
#include "utils.h"

bool isBoardDisabled() {
  pinMode(PIN_GUARD, INPUT_PULLUP);
  int val = digitalRead(PIN_GUARD);
  return val == HIGH;
}

void setupPins() {
  pinMode(PIN_LED_SETUP, OUTPUT);
  pinMode(PIN_LED_MASTER, OUTPUT);
  pinMode(PIN_BTN_SETUP_MODE, INPUT_PULLUP);
  pinMode(PIN_BTN_MASTER_KEY, INPUT_PULLUP);
}

bool isMagicPresent() {
  uint16_t val = 0;
  EEPROM.get(EEPROM_MAGIC_OFFSET, val);
  return val == EEPROM_MAGIC;
}

uint16_t getPasswdLen() {
  if (!isMagicPresent()) {
    return 0;
  }

  uint16_t val = 0;
  EEPROM.get(EEPROM_PASSWD_LEN_OFFSET, val);
  return val;
}

void savePasswd(uint16_t len, char* data) {
  // Write magic
  if (!isMagicPresent()) {
    EEPROM.put(EEPROM_MAGIC_OFFSET, EEPROM_MAGIC);
  }

  // Commit len
  EEPROM.put(EEPROM_PASSWD_LEN_OFFSET, len);

  for (uint16_t i = 0; i < len; i++) {
    EEPROM.update(EEPROM_PASSWD_DATA_OFFSET + i, data[i]);
  }
}

void readPasswdStr(uint16_t len, char* dst) {
  if (len == 0) {
    return;
  }

  for (uint16_t i = 0; i < len; i++) {
    dst[i] = EEPROM.read(EEPROM_PASSWD_DATA_OFFSET + i);
  }
}

void wipePasswd() {
  for (uint16_t i = EEPROM_PASSWD_OFFSET; i < EEPROM_PASSWD_DATA_OFFSET + 1; i++) {
    EEPROM.update(i, 0xFF);
  }
}
