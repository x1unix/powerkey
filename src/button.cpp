#include <Arduino.h>
#include "button.h"

Button::Button(uint8_t pin) : Button(pin, 100) {}
Button::Button(uint8_t pin, unsigned long debounceInterval) {
  m_state = false;
  m_prevState = false;
  m_prevMillis = 0;
  m_pin = pin;
  m_debounceInterval = debounceInterval;
  pinMode(pin, INPUT_PULLUP);
}

void Button::update() {
  auto now = millis();
  auto delta = now - m_prevMillis;
  if (delta < m_debounceInterval) {
    return;
  }

  m_prevMillis = now;
  auto current = digitalRead(m_pin) == LOW;
  m_prevState = m_state;
  m_state = current;
}

bool Button::isActivated() {
  update();
  bool wasActivated = m_prevState && !m_state;
  if (wasActivated) {
    m_prevState = false;
  }

  return wasActivated;
}

bool Button::isKeyDown() {
  return m_state;
}
