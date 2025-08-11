#include <Arduino.h>
#include <EEPROM.h>
#include "config.h"
#include "utils.h"
#include "state.h"
#include "saga.h"

class NoOpSaga : public Saga {
  void enter(BoardState& state) override {}
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

#define SETUP_BLINK_INTERVAL 100

class SetupSaga : public Saga {
private:
  unsigned long prevMillis = 0;
public:
  void enter(BoardState& state) override {
    prevMillis = millis();
    digitalWrite(PIN_LED_SETUP, HIGH);
  }

  void leave(BoardState& state) override {
    digitalWrite(PIN_LED_SETUP, LOW);
  }

  ActionType tick(BoardState& state) override {
    unsigned long now = millis();
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
  return BoardState{
    .isDisabled = isBoardDisabled(),
    .buttons = ButtonsState{
      .masterKey = TcBUTTON(PIN_BTN_MASTER_KEY),
      .setup = TcBUTTON(PIN_BTN_SETUP_MODE) }
  };
}

Saga* firstSaga(BoardState& state) {
  return nextSaga(ActionType::BOOT, state, nullptr);
}

Saga* sagaFromActionType(ActionType action) {
  switch (action) {
    case ActionType::EMPTY:
      // TODO: add panic saga
      return nullptr;
    case ActionType::BOOT:
      return new BootSaga();
    case ActionType::KEYBOARD_IDLE:
      return new KeyboardSaga();
    case ActionType::SETUP_ENTER:
      return new SetupSaga();
    case ActionType::DISABLE_BOARD:
      return new NoOpSaga();
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

  Saga* nextSaga = sagaFromActionType(action);
  nextSaga->enter(state);
  return nextSaga;
}
