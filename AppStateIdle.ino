// This file declares and implements the State* classes, which are the child of State class
// and contains the logic of each state of our application.
//
// Pierre Rossel   2024-04-26  Initial version

void StateIdle::enter()
{
  Serial.println(">>>>>>>>>>>>> StateIdle.enter()");
  pinMode(LED_BUILTIN, OUTPUT);

  // theaterChase(strip.Color(127, 127, 127), 50); // White
  // strip.clear();
  // strip.show();
  // delay(100);
  // theaterChase(strip.Color(255, 255, 255), 50); // White

  strip.clear();
  strip.show();

  // read accelerometer when entering the state
  IMU.readAcceleration(aXEnter, aYEnter, aZEnter);

}

State *StateIdle::loop()
{
  // Every second, turn the next LED on or off on the strip
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 1000)
  {
    lastUpdate = millis();
    static int led = 0;
    strip.clear();
    strip.setPixelColor(led, strip.Color(255, 255, 255));
    strip.show();
    led++;
    if (led >= strip.numPixels())
    {
      led = 0;
    }
  }

  cycleRGBLed();

  // read accelerometer. If the angle between the current and the enter angle is greater than 10 degrees, switch to the StatePlay
  float aX, aY, aZ;
  IMU.readAcceleration(aX, aY, aZ);
  
  // angle between the two vectors [aX, aY, aZ] and [axEnter, ayEnter, azEnter]
  float angle = acos((aX * aXEnter + aY * aYEnter + aZ * aZEnter) / (sqrt(aX * aX + aY * aY + aZ * aZ) * sqrt(aXEnter * aXEnter + aYEnter * aYEnter + aZEnter * aZEnter))) * 180 / PI;

  // Serial.print("angle: ");
  // Serial.println(angle);
  if (angle > 10)
  {
    return new StatePlay();
  }

  delay(100);

  return checkSerial();
  // return getStateTime() > 5000 ? new StateGlow() : NULL;
}

void StateIdle::exit()
{
  Serial.println("<<<<<<<<<<<<< StateIdle.exit()");
  strip.clear();
  strip.show();
}

void StateIdle::cycleRGBLed()
{
  static unsigned long lastUpdate = 0;
  static int hue = 0;
  static int saturation = 100;
  static int brightness = 100;

  // if last update time is not set, or the difference between now and last update time is greater than 3 seconds
  if (lastUpdate == 0 || millis() - lastUpdate > 10)
  {
    lastUpdate = millis();

    // convert hue to RGB
    int r, g, b;
    HSBtoRGB(hue, saturation, brightness, r, g, b);

    // set the RGB LED
    analogWrite(LEDR, 255 - r);
    analogWrite(LEDG, 255 - g);
    analogWrite(LEDB, 255 - b);

    // increase the hue by 1
    hue++;

    // reset the hue after it reaches 360
    if (hue >= 360)
    {
      hue = 0;
    }
  }
}
