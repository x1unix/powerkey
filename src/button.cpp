#include <Arduino.h>
#include "button.h"

Button::Button(uint8_t pin) : Button(pin, 100) {}
Button::Button(uint8_t pin, unsigned long debounceInterval) {
  m_pin = pin;
  m_debounceInterval = debounceInterval;
}

void Button::update() {
  auto now = millis();
  auto delta = now - m_prevMillis;
  if (delta < m_debounceInterval) {
    return;
  }

  m_prevMillis = now;
  auto current = digitalRead(m_pin) == LOW;
  if (current != m_state) {
    m_prevState = m_state;
    m_state = current;
  }
}

bool Button::isActivated() {
  update();
  return m_prevState && !m_state;
}

bool Button::isKeyDown() {
  return m_state;
}
