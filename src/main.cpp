#include "config.h"
#include <Arduino.h>
#include <Keyboard.h>
#include "config.h"

void setup() {
  pinMode(PIN_LED_SETUP, OUTPUT);
  pinMode(PIN_LED_MASTER, OUTPUT);
  pinMode(PIN_BTN_SETUP_MODE, INPUT_PULLUP);
  pinMode(PIN_BTN_MASTER_KEY, INPUT_PULLUP);
}

uint8_t br0 = 0;
uint8_t br1 = 0xff;
int d0 = 1;
int d1 = -1;

void upd_pin(uint8_t pin, uint8_t *val, int *delta) {
  uint8_t cur_val = *val;
  analogWrite(pin, cur_val);

  switch (*val) {
    case 0:
      *delta = 1;
      break;
    case 0xff:
      *delta = -1;
    default:
      break;
  }

  *val = byte(int(cur_val) + *delta);
}

void loop() {
  upd_pin(PIN_LED_MASTER, &br0, &d0);
  upd_pin(PIN_LED_SETUP, &br1, &d1);
  delay(10);
}
