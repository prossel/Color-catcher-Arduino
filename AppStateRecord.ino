// This file declares and implements the State* classes, which are the child of State class
// and contains the logic of each state of our application.
//
// Pierre Rossel   2024-04-26  Initial version

void StateRecord::enter()
{
  Serial.println(">>>>>>>>>>>>> StateRecord.enter()");
}

State *StateRecord::loop()
{

  dumpAccelerometer();

  return checkSerial();
}

void StateRecord::exit()
{
  Serial.println("<<<<<<<<<<<<< StateRecord.exit()");
}

void StateRecord::dumpAccelerometer()
{
  // read accelerometer
  float x, y, z;
  if (IMU.accelerationAvailable())
  {

    IMU.readAcceleration(x, y, z);

    Serial.print("X:");
    Serial.print(x);
    Serial.print("\t");
    Serial.print("Y:");
    Serial.print(y);
    Serial.print("\t");
    Serial.print("Z:");
    Serial.println(z);
  }
}
