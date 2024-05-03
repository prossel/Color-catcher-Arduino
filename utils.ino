
State *checkSerial()
{
  if (Serial.available() > 0)
  {
    char c = Serial.read();
    if (c == 'i')
    {
      return new StateIdle();
    }
    if (c == 'r')
    {
      return new StateRecord();
    }
    if (c == 'p')
    {
      return new StatePlay();
    }
  }
  return NULL;
}

// convert hue, saturation and brightness to RGB
// hue: 0-360, saturation: 0-100, brightness: 0-100
// r, g, b: 0-255
void HSBtoRGB(int hue, int saturation, int brightness, int &r, int &g, int &b) {
  int h = round(hue / 60.0);
  int s = round(saturation / 100.0 * 255);
  int v = round(brightness / 100.0 * 255);

  int c = s * v / 255;
  int x = c * (1 - abs((h % 2) - 1));
  int m = v - c;

  int r1, g1, b1;
  if (h == 0) {
    r1 = c;
    g1 = x;
    b1 = 0;
  } else if (h == 1) {
    r1 = x;
    g1 = c;
    b1 = 0;
  } else if (h == 2) {
    r1 = 0;
    g1 = c;
    b1 = x;
  } else if (h == 3) {
    r1 = 0;
    g1 = x;
    b1 = c;
  } else if (h == 4) {
    r1 = x;
    g1 = 0;
    b1 = c;
  } else if (h == 5) {
    r1 = c;
    g1 = 0;
    b1 = x;
  }

  r = r1 + m;
  g = g1 + m;
  b = b1 + m;
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait)
{
    for (int a = 0; a < 10; a++)
    { // Repeat 10 times...
        for (int b = 0; b < 3; b++)
        {                  //  'b' counts from 0 to 2...
            strip.clear(); //   Set all pixels in RAM to 0 (off)
            // 'c' counts up from 'b' to end of strip in steps of 3...
            for (int c = b; c < strip.numPixels(); c += 3)
            {
                strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
            }
            strip.show(); // Update strip with new contents
            delay(wait);  // Pause for a moment
        }
    }
}
