#ifndef _H_CONFIG
#define _H_CONFIG
#include <Arduino.h>

#if defined(ARDUINO_ARCH_AVR) || defined(__AVR__)
#define ANALOG_MAX 1023
#elif defined(ARDUINO_ARCH_SAMD)
#define ANALOG_MAX 4095
#else
#error "ANALOG_MAX: Unsupported board"
#endif

// Uncomment this to enable debug logging
//#define DEBUG

// Maximum password length in ASCII chars.
//
// Depends on EEPROM capacity of a board.
#define MAX_PASSWD_LEN 255

// Pin that controls whether Arduino should act as a USB HID device.
// In order to reflash a board, disconnect the pin from the ground.
#define PIN_GUARD 8

// Pin to put device into setup mode.
// Setup mode is used to set keyboard strokes to send on key press.
//
// Note: should be an interruptable pin.
#define PIN_BTN_SETUP_MODE PIN3

// Pin to handle the master key that sends keystrokes.
//
// Note: should be an interruptable pin.
#define PIN_BTN_MASTER_KEY PIN2

// Pin to drive LED active during master key mode.
#define PIN_LED_MASTER PIN5

// Pin to drive LED active during setup mode.
#define PIN_LED_SETUP PIN6

#endif
