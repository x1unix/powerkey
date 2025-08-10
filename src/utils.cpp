#include "utils.h"
#include <Arduino.h>
#include <EEPROM.h>
#include "config.h"

bool g_isInitialized = false;
bool g_isDisabled = false;
BoardState g_state = BoardState::DISABLED;
uint16_t g_passwdLen = 0;

void initState() {
  // Check if there is any password in EEPROM.
  // Otherwize - prompt user to setup a new one.
  EEPROM.get(EEPROM_PASSWD_LEN_OFFSET, g_passwdLen);

  if (!g_passwdLen) {
    g_state = BoardState::PASSWD_EMPTY;
  }
}

void boostrap() {
  pinMode(PIN_GUARD, INPUT_PULLUP);
  int val = digitalRead(PIN_GUARD);
  g_isDisabled = val == HIGH;
  if (!g_isDisabled) {
    initState();
  }

  g_isInitialized = true;
}

bool isBoardDisabled() {
  if (!g_isInitialized) {
    boostrap();
  }

  return g_isDisabled;
}
