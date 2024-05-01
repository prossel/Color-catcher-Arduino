// This file declares and implements the State* classes, which are the child of State class
// and contains the logic of each state of our application.
//
// Pierre Rossel   2024-04-26  Initial version

void StatePlay::enter()
{
  Serial.println(">>>>>>>>>>>>> StatePlay.enter()");
}

State *StatePlay::loop()
{
  return checkSerial();
}

void StatePlay::exit()
{
  Serial.println("<<<<<<<<<<<<< StatePlay.exit()");
}
