#include <Arduino.h>
#include <EEPROM.h>
#include "config.h"
#include "utils.h"
#include "state.h"
#include "saga.h"

class NoOpSaga : public Saga {
  void enter(BoardState& state) override {
    digitalWrite(PIN_LED_SETUP, HIGH);
  }
  void leave(BoardState& state) override {}
  ActionType tick(BoardState& state) override {
    return ActionType::EMPTY;
  }
};

class BootSaga : public Saga {
  void leave(BoardState& state) override {}
  void enter(BoardState& state) override {
    if (state.isDisabled) {
      return;
    }

    // Configure LEDs and button pins.
    setupPins();

    // Check if there is any password in EEPROM.
    // Otherwize - prompt user to setup a new one.
    EEPROM.get(EEPROM_PASSWD_LEN_OFFSET, state.pwdLen);
  }

  ActionType tick(BoardState& state) override {
    if (!state.isDisabled) {
      return ActionType::DISABLE_BOARD;
    }

    return state.pwdLen == 0 ? ActionType::SETUP_ENTER : ActionType::KEYBOARD_IDLE;
  }
};

#define SETUP_BLINK_INTERVAL 300

class SetupSaga : public Saga {
private:
  unsigned long m_prevMillis = 0;
  bool m_ledEnabled = false;

  void updateLedState(bool state) {
    m_prevMillis = millis();
    digitalWrite(PIN_LED_SETUP, state ? HIGH : LOW);
    m_ledEnabled = state;
  }

  void blinkLed() {
    unsigned long now = millis();
    if (now - m_prevMillis < SETUP_BLINK_INTERVAL) {
      return;
    }

    updateLedState(!m_ledEnabled);
  }
public:
  void enter(BoardState& state) override {
    Serial.begin(9600);
    updateLedState(true);
  }

  void leave(BoardState& state) override {
    digitalWrite(PIN_LED_SETUP, LOW);
  }

  ActionType tick(BoardState& state) override {
    blinkLed();
    return ActionType::EMPTY;
  }
};

class KeyboardSaga : public Saga {
  void enter(BoardState& state) override {}
  void leave(BoardState& state) override {}
  ActionType tick(BoardState& state) override {
    return ActionType::EMPTY;
  }
};

BoardState getInitialState() {
  // Arduino's compiler doesn't support designated initializers :(
  return BoardState{
    ActionType::EMPTY,
    ActionType::EMPTY,
    isBoardDisabled(),
    false,
    false,
    { TcBUTTON(PIN_BTN_MASTER_KEY),
      TcBUTTON(PIN_BTN_SETUP_MODE) }
  };
}

Saga* firstSaga(BoardState& state) {
  return nextSaga(ActionType::BOOT, state, nullptr);
}

Saga* sagaFromActionType(ActionType action) {
  switch (action) {
    case ActionType::BOOT:
      return new BootSaga();
    case ActionType::KEYBOARD_IDLE:
      return new KeyboardSaga();
    case ActionType::SETUP_ENTER:
      return new SetupSaga();
    case ActionType::DISABLE_BOARD:
      return new NoOpSaga();
    default:
      // TODO: add panic saga
      return nullptr;
  }
}

Saga* nextSaga(ActionType action, BoardState& state, Saga* prevSaga) {
  if (action == ActionType::EMPTY) {
    return prevSaga;
  }

  state.prevAction = state.currentAction;
  state.currentAction = action;
  if (prevSaga != nullptr) {
    prevSaga->leave(state);
  }

  Serial.print("nextSaga: ");
  Serial.println(action);
  Saga* nextSaga = sagaFromActionType(action);
  nextSaga->enter(state);
  return nextSaga;
}
