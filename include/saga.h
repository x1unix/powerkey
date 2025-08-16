#ifndef _H_SAGA
#define _H_SAGA
#include "state.h"


/**
 * Abstract saga to manage current state and transition between different device operation stages.
 */
class Saga {
public:
  /**
   * Called once when state enters to a new saga.
   */
  virtual void enter(BoardState &state) = 0;

  /**
   * Tick performs operation per CPU cycle over a state.
   *
   * Returns next action when necessary to transfer to another state stage.
   * Otherwize, returns [ActionType.EMPTY].
   */
  virtual ActionType tick(BoardState &state) = 0;

  /**
   * Disposes saga when state transitions to a next saga.
   *
   * Next action type for a saga is available at [BoardState.currentAction].
   */
  virtual void leave(BoardState &state) = 0;

  virtual ~Saga() = default;
};

/**
 * Constructs and returns initial state.
 */
BoardState getInitialState();

/**
 * Transitions and returns the first saga.
 */
Saga *firstSaga(BoardState &state);

/**
 * Takes next action and advances to a next saga.
 *
 * Stays on a current saga if action is [ActionType::EMPTY].
 */
Saga *nextSaga(ActionType action, BoardState &state, Saga *prevSaga);

#endif
