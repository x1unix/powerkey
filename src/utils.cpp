#include <Arduino.h>
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
