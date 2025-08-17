#ifndef _H_STATE
#define _H_STATE
#include <Arduino.h>
#include "button.h"

enum ActionType {
  EMPTY = 0,
  BOOT = 1,
  DISABLE_BOARD = 2,
  SETUP_ENTER = 3,
  KEYBOARD_IDLE = 4,
};

struct ButtonsState {
  Button masterKey;
  Button setup;
};

/**
 * Global board state.
 */
struct BoardState {
  ActionType currentAction;
  ActionType prevAction;
  bool isDisabled;
  bool isInitialized;
  uint16_t pwdLen;
  uint8_t* pwdData;
  ButtonsState buttons;
};

#endif
