#include "config.h"
#include <Arduino.h>
#include <Keyboard.h>
#include <TcBUTTON.h>

#include "config.h"
#include "utils.h"

TcBUTTON g_btnMasterKey(PIN_BTN_MASTER_KEY);
TcBUTTON g_btnSetupMode(PIN_BTN_SETUP_MODE);

void setup() {
  pinMode(PIN_LED_SETUP, OUTPUT);
  pinMode(PIN_LED_MASTER, OUTPUT);

  if (isBoardDisabled()) {
    return;
  }

  pinMode(PIN_BTN_SETUP_MODE, INPUT_PULLUP);
  pinMode(PIN_BTN_MASTER_KEY, INPUT_PULLUP);
}

void loop() {
  if (isBoardDisabled()) {
    return;
  }
}
