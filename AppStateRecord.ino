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
  unsigned long currentMillis = millis();
  static unsigned long lastMove = 0;

  // read accelerometer
  float aX, aY, aZ;
  if (IMU.accelerationAvailable())
  {

    IMU.readAcceleration(aX, aY, aZ);

    // sum up the absolutes
    float aSum = fabs(aX) + fabs(aY) + fabs(aZ);

    float aModule = sqrt(aX * aX + aY * aY + aZ * aZ);

    bool isMoving = abs(aModule - 1) > 0.1;

    if (isMoving) {
      lastMove = currentMillis;
    }

    if (currentMillis < lastMove + 100)
    {
      Serial.print("aX:");
      Serial.print(aX);
      Serial.print("\t");
      Serial.print("aY:");
      Serial.print(aY);
      Serial.print("\t");
      Serial.print("aZ:");
      Serial.print(aZ);
      Serial.print("\t");
      Serial.print("aSum:");
      Serial.print(aSum);
      Serial.print("\t");
      Serial.print("aModule:");
      Serial.print(aModule);

      Serial.println();
    }
  }
}
