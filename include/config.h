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

// Pin that controls whether Arduino should act as a USB HID device.
// In order to reflash a board, disconnect the pin from the ground.
#define PIN_KBD_JUMPER PIN2

// Pin to put device into setup mode.
//
// Setup mode is used to set keyboard strokes to send on key press.
#define PIN_SETUP_MODE PIN4

// Pin to handle the master key that sends keystrokes.
#define PIN_MASTER_KEY 8

// Pin to drive LED active during master key mode.
#define PIN_LED_MASTER 14

// Pin to drive LED active during setup mode.
#define PIN_LED_SETUP 15

#endif
