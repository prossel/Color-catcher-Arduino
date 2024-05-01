// This file defines and implements the base classe of a State and the StateMachine class.
//
// Create your own child classes from State and use them with an instance of StateMachine
//
// Pierre Rossel   2020-12-03  Initial version

#pragma once

class State {

  public:

    unsigned long stateEnterMs = 0;

    // Override if you need to do something when the state enters.
    virtual void enter() {}

    // Override this function to execute what your state has to do when active.
    // Return NULL or a new State if you want to change state.
    virtual State * loop() {}

    // Override this function if you need to do some cleanup before your state is destroyed
    // Called before switching to another state
    virtual void exit() {}

    // Get the time in ms elapsed since state entered.
    unsigned long getStateTime() {
      return millis() - stateEnterMs;
    }
};


class StateMachine {

    State *pCurrentState;

  public:

    void loop() {
      if (pCurrentState != NULL) {
        State *pNewState = pCurrentState->loop();
        if (pNewState) {
          changeState(pNewState);
        }
      }
    }

    // Change to another state.
    // Note current state will be called its exit() function, then destroyed.
    void changeState(State *pNewState) {

      if (pCurrentState != NULL) {
        pCurrentState->exit();
        delete pCurrentState;
        pCurrentState = NULL;
      }

      pCurrentState = pNewState;

      if (pNewState != NULL) {
        pNewState->stateEnterMs = millis();
        pNewState->enter();
      }
    }

    // returns current State
    State * getState() {
      return pCurrentState;
    }

    // Returns the running time of current state in milliseconds.
    unsigned long getStateTime() {
      return pCurrentState ? pCurrentState->getStateTime() : 0;
    }

};
