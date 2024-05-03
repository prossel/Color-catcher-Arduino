// This file declares and implements the State* classes, which are the child of State class
// and contains the logic of each state of our application.
//
// Pierre Rossel   2024-04-26  Initial version

#include "StateMachine.h"

class StateIdle : public State
{

  float aXEnter, aYEnter, aZEnter = 0;

  void enter();

  // Loop is declared here, but implemented below, after the declaration of other states
  State *loop();

  void exit();

  void cycleRGBLed();
};

class StateRecord : public State
{

  void enter();

  // Loop is declared here, but implemented below, after the declaration of other states
  State *loop();

  void exit();

  void dumpAccelerometer();
};

class StatePlay : public State
{

  void enter();

  // Loop is declared here, but implemented below, after the declaration of other states
  State *loop();

  void exit();
};


