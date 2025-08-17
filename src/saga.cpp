#include <Arduino.h>
#include <Keyboard.h>
#include "logger.h"
#include "config.h"
#include "utils.h"
#include "state.h"
#include "prompt.h"
#include "saga.h"

class NoOpSaga : public Saga {
  void enter(BoardState& state) override {
    pinMode(PIN_GUARD, OUTPUT);  // To keep lights on.

    wipePasswd();
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
    state.pwdLen = getPasswdLen();
  }

  ActionType tick(BoardState& state) override {
    if (state.isDisabled) {
      return ActionType::DISABLE_BOARD;
    }

    return state.pwdLen == 0 ? ActionType::SETUP_ENTER : ActionType::KEYBOARD_IDLE;
  }
};

#define SETUP_BLINK_INTERVAL 300

class SetupSaga : public Saga {
public:
  SetupSaga() : m_reader(MAX_PASSWD_LEN) {
  }

private:
  PromptReader m_reader;
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

  ActionType checkSerial(BoardState& state) {
    if (!Serial) {
      return ActionType::EMPTY;
    }

    if (m_reader.read()) {
      // Consume input and continue.
      return ActionType::EMPTY;
    }

    auto result = m_reader.getResult();
    if (result.len == 0) {
      DEBUG_LOG("No password");
      return state.prevAction == ActionType::BOOT ? ActionType::EMPTY : state.prevAction;
    }

#ifdef DEBUG
    Serial.print("Received: len=");
    Serial.print(result.len);
    Serial.print("; data=");
    Serial.write(result.data, result.len);
    Serial.println();
#endif

    savePasswd(result.len, result.data);
    state.pwdLen = result.len;
    if (state.pwdData != nullptr) {
      delete[] state.pwdData;
    }

    return state.prevAction == ActionType::BOOT ? ActionType::KEYBOARD_IDLE : state.prevAction;
  }

public:
  void enter(BoardState& state) override {
    Serial.begin(9600);
    updateLedState(true);
    m_reader.reset();
  }

  void leave(BoardState& state) override {
    Serial.end();
    m_reader.reset();
    digitalWrite(PIN_LED_SETUP, LOW);
  }

  ActionType tick(BoardState& state) override {
    blinkLed();
    if (state.buttons.setup.isActivated()) {
      // Abort on button press
      return state.prevAction;
    }

    return checkSerial(state);
  }
};

class KeyboardSaga : public Saga {
  void enter(BoardState& state) override {
#ifdef DEBUG
    Serial.begin(9600);
#else
    Keyboard.begin();
#endif

    state.pwdLen = getPasswdLen();

    if (state.pwdLen == 0) {
      return;
    }

    state.pwdData = new uint8_t[state.pwdLen];
    readPasswdStr(state.pwdLen, state.pwdData);
    digitalWrite(PIN_LED_MASTER, HIGH);
#ifdef DEBUG
    Serial.print("KeyboardSaga: load passwd: len=");
    Serial.print(state.pwdLen);
    Serial.print("; data=");
    Serial.write(state.pwdData, state.pwdLen);
    Serial.println();
#endif
  }

  void leave(BoardState& state) override {
    Serial.end();
    digitalWrite(PIN_LED_MASTER, LOW);
  }

  ActionType tick(BoardState& state) override {
    if (state.pwdLen == 0) {
      return ActionType::SETUP_ENTER;
    }

    if (state.buttons.setup.isActivated()) {
      return ActionType::SETUP_ENTER;
    }

    if (state.buttons.masterKey.isActivated()) {
#ifdef DEBUG
      Serial.print("PWD: ");
      Serial.write(state.pwdData, state.pwdLen);
      Serial.println();
#else
      Keyboard.write((uint8_t*)state.pwdData, (size_t)state.pwdLen);
#endif
    }
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
    nullptr,
    { Button(PIN_BTN_MASTER_KEY),
      Button(PIN_BTN_SETUP_MODE) }
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
    delete prevSaga;
    prevSaga = nullptr;
  }

  DEBUG_LOG("nextSaga: ", action);
  Saga* nextSaga = sagaFromActionType(action);
  nextSaga->enter(state);
  return nextSaga;
}
