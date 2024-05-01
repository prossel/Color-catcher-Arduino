// This file declares and implements the State* classes, which are the child of State class
// and contains the logic of each state of our application.
//
// Pierre Rossel   2024-04-26  Initial version

void StateIdle::enter()
{
  Serial.println(">>>>>>>>>>>>> StateIdle.enter()");
  pinMode(LED_BUILTIN, OUTPUT);
}

State *StateIdle::loop()
{

  cycleRGBLed();

  return checkSerial();
  // return getStateTime() > 5000 ? new StateGlow() : NULL;
}

void StateIdle::exit()
{
  Serial.println("<<<<<<<<<<<<< StateIdle.exit()");
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
