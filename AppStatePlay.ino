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
  unsigned long currentMillis = millis();
  static unsigned long lastMoving = 0;
  static unsigned long lastValidMove = 0;
  static float aZFast = 0;
  static float aZSlow = 0;
  static float speedZ = 0;

  // read accelerometer
  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable())
  {
    float aX, aY, aZ;
    IMU.readAcceleration(aX, aY, aZ);

    // Flip z axis because the sensor is finally mounted reversed in the object
    aZ *= -1;

    // read gyroscope
    float gX, gY, gZ;
    IMU.readGyroscope(gX, gY, gZ);

    // Send the data to the monitor characteristic every 100 ms
    static unsigned long lastMonitor = 0;
    if (currentMillis > lastMonitor + 100)
    {
      lastMonitor = currentMillis;

      String data = String(aX) + "," + String(aY) + "," + String(aZ) + "," + String(gX) + "," + String(gY) + "," + String(gZ);  
      monitorCharacteristic.writeValue(data);
    }

    // filter the acceleration
    float alphaFast = 0.6;
    float alphaSlow = 0.95;
    aZFast = alphaFast * aZFast + (1 - alphaFast) * aZ;
    aZSlow = alphaSlow * aZSlow + (1 - alphaSlow) * aZ;

    // remove the gravity
    float aZReal = aZFast - aZSlow;

    // speed
    speedZ = speedZ + aZReal * 0.5;

    // compensate the drift and the rotation
    speedZ = speedZ * 0.6;

    static float lastSpeedZ = 0;
    float threshold = 0.25;

    // float angle = atan2(aY, aZSlow) * 180 / PI;
    float angle = int(atan2(-aY, aZSlow) * 180 / PI + 360 ) % 360;
    // float angle = atan2(aZSlow, aY) * 180 / PI;

    // Detect a cross down high followed by a cross down low
    static unsigned long lastMsCrossDownHigh = 0;
    if (speedZ < threshold && lastSpeedZ >= threshold)
    {
      lastMsCrossDownHigh = currentMillis;
    }

    // cross down low
    if (speedZ < -threshold && lastSpeedZ >= -threshold)
    {
      if (currentMillis < lastMsCrossDownHigh + 200)
      {
        lastValidMove = currentMillis;

        byte move = angle / 60;
        moveCharacteristic.writeValue(move);

        // Serial.println("Cross High - Low duration: ");
        // Serial.println(currentMillis - lastMsCrossDownHigh);
        Serial.print(" Move detected_");
        // Serial.println(angle);
        Serial.println(move);


        // Serial.print("aX:  ");
        // Serial.print(aX);
        // Serial.print("\t");

        // Serial.print("aY:  ");
        // Serial.print(aY);
        // Serial.print("\t");

        // Serial.print("aZ:  ");
        // Serial.print(aZ);
        // Serial.print("\t");
      }
    }

    // // Cross up low
    // static unsigned long lastMsCrossUpLow = 0;
    // if (speedZ > -threshold && lastSpeedZ < -threshold)
    // {
    //   lastMsCrossUpLow = currentMillis;
    // }

    // // cross up high
    // if (speedZ > threshold && lastSpeedZ < threshold)
    // {
    //   if (currentMillis > lastMsCrossUpLow + 500)
    //   {
    //     lastValidMove = currentMillis;
    //     Serial.println("Move detected");
    //   }
    // }

    lastSpeedZ = speedZ;

    // check if the device is moving
    bool isMoving = abs(aZReal) > 0.5;
    // bool isMoving = abs(speedZ) > 0.1;
    // bool isMoving = 0;

    if (isMoving)
    {
      lastMoving = currentMillis;
    }

    // switch to the StateIdle if the device is not moving
    if (currentMillis > lastMoving + 10000 && getStateTime() > 10000)
    {
      return new StateIdle();
    }


    if (currentMillis < lastMoving + 100)
    {
      Serial.print("min:-2\tmax:2\t");

      // Serial.print("aX:");
      // Serial.print(aX);
      // Serial.print("\t");

      Serial.print("aY:");
      Serial.print(aY);
      Serial.print("\t");

      Serial.print("aZ:");
      Serial.print(aZ);
      Serial.print("\t");

      Serial.print("ValidMove:");
      Serial.print(lastValidMove == currentMillis);
      Serial.print("\t");

      // Serial.print("aZFast:");
      // Serial.print(aZFast);
      // Serial.print("\t");

      Serial.print("aZSlow:");
      Serial.print(aZSlow);
      Serial.print("\t");

      Serial.print("angle:");
      Serial.print(angle);
      Serial.print("\t");

      // Serial.print("aZFastDiff:");
      // Serial.print(aZ - aZFast);
      // Serial.print("\t");

      // Serial.print("aZSlowDiff:");
      // Serial.print(aZ - aZSlow);
      // Serial.print("\t");

      // Serial.print("speedZ:");
      // Serial.print(speedZ);
      // Serial.print("\t");

      // Serial.print("aZReal:");
      // Serial.print(aZReal);
      // Serial.print("\t");

      Serial.println();
    }
  }

  delay(10);

  return checkSerial();
}

void StatePlay::exit()
{
  Serial.println("<<<<<<<<<<<<< StatePlay.exit()");
}
