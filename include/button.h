#ifndef H_BUTTON
#define H_BUTTON
#include <Arduino.h>

/**
 * A NIH helper to read buttons state.
 *
 * Why? Because having a yet-another-button-lib seems like a brilliant idea.
 */
class Button {
private:
  bool m_prevState;
  bool m_state;
  unsigned long m_prevMillis;
  unsigned long m_debounceInterval;
protected:
  uint8_t m_pin;
  void update();
public:
  Button(uint8_t pin);
  Button(uint8_t pin, unsigned long debounceInterval);

  /**
   * Returns if key was pressed and released.
   *
   * Similar to how "onClick" events work in GUI frameworks.
   */
  bool isActivated();

  /**
   * Returns whether key is pressed in this moment.
   */
  bool isKeyDown();
};
#endif
