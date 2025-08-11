#include <Arduino.h>
#include <Keyboard.h>
#include "state.h"
#include "saga.h"

BoardState g_state = getInitialState();
Saga* g_saga;

void setup() {
  g_saga = firstSaga(g_state);
}

void loop() {
  ActionType action = g_saga->tick(g_state);
  g_saga = nextSaga(action, g_state, g_saga);
}
