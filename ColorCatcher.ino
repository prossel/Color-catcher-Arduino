/*
  Button LED

  This example creates a Bluetooth® Low Energy peripheral with service that contains a
  characteristic to control an LED and another characteristic that
  represents the state of the button.

  The circuit:
  - Arduino MKR WiFi 1010, Arduino Uno WiFi Rev2 board, Arduino Nano 33 IoT,
    Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.
  - Button connected to pin 4

  You can use a generic Bluetooth® Low Energy central app, like LightBlue (iOS and Android) or
  nRF Connect (Android), to interact with the services and characteristics
  created in this sketch.

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>

#include <Arduino_LSM9DS1.h>

const int ledPin = LED_BUILTIN; // set ledPin to on-board LED
const int buttonPin = 4;        // set buttonPin to digital pin 4

BLEService ledService("19B10010-E8F2-537E-4F6C-D104768A1214"); // create service

// create switch characteristic and allow remote device to read and write
BLEByteCharacteristic ledCharacteristic("19B10011-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
// create button characteristic and allow remote device to get notifications
BLEByteCharacteristic buttonCharacteristic("19B10012-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);

#include "AppStates.h"

StateMachine sm;

void setup()
{

  // Serial
  Serial.begin(9600);
  while (!Serial && millis() < 4000)
    ;
  Serial.println("Started");

  // GPIO
  pinMode(ledPin, OUTPUT);          // use the LED as an output
  pinMode(buttonPin, INPUT_PULLUP); // use button pin as an input

  // Accelerometer
  if (!IMU.begin())
  {
    Serial.println("Failed to initialize IMU!");
    while (1)
      ;
  }

  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println("Hz");

  // begin initialization
  if (!BLE.begin())
  {
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while (1)
      ;
  }

  // set the local name peripheral advertises
  BLE.setLocalName("Color catcher");

  BLE.setDeviceName("Color catcher");

  // set the UUID for the service this peripheral advertises:
  BLE.setAdvertisedService(ledService);

  // add the characteristics to the service
  ledService.addCharacteristic(ledCharacteristic);
  ledService.addCharacteristic(buttonCharacteristic);

  // add the service
  BLE.addService(ledService);

  ledCharacteristic.writeValue(0);
  buttonCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth® device active, waiting for connections...");

  sm.changeState(new StateIdle());
}

void loop()
{
  // poll for Bluetooth® Low Energy events
  BLE.poll();

  // read the current button pin state
  char buttonValue = digitalRead(buttonPin);

  // has the value changed since the last read
  bool buttonChanged = (buttonCharacteristic.value() != buttonValue);

  if (buttonChanged)
  {
    // button state changed, update characteristics
    ledCharacteristic.writeValue(buttonValue);
    buttonCharacteristic.writeValue(buttonValue);
  }

  if (ledCharacteristic.written() || buttonChanged)
  {
    // update LED, either central has written to characteristic or button state has changed
    if (ledCharacteristic.value())
    {
      Serial.println("LED on");
      digitalWrite(ledPin, HIGH);
    }
    else
    {
      Serial.println("LED off");
      digitalWrite(ledPin, LOW);
    }
  }

  // cycleRGBLed();

  // dumpAccelerometer();

  sm.loop();
}

// void dumpAccelerometer() {
//   // read accelerometer
//   float x, y, z;
//   if (IMU.accelerationAvailable()) {

//     IMU.readAcceleration(x, y, z);

//     Serial.print("X:");
//     Serial.print(x);
//     Serial.print("\t");
//     Serial.print("Y:");
//     Serial.print(y);
//     Serial.print("\t");
//     Serial.print("Z:");
//     Serial.println(z);
//   }
// }

// void cycleRGBLed() {
//   static unsigned long lastUpdate = 0;
//   static int hue = 0;
//   static int saturation = 100;
//   static int brightness = 100;

//   // if last update time is not set, or the difference between now and last update time is greater than 3 seconds
//   if (lastUpdate == 0 || millis() - lastUpdate > 10) {
//     lastUpdate = millis();

//     // convert hue to RGB
//     int r, g, b;
//     HSBtoRGB(hue, saturation, brightness, r, g, b);

//     // set the RGB LED
//     analogWrite(LEDR, 255 - r);
//     analogWrite(LEDG, 255 - g);
//     analogWrite(LEDB, 255 - b);

//     // increase the hue by 1
//     hue++;

//     // reset the hue after it reaches 360
//     if (hue >= 360) {
//       hue = 0;
//     }
//   }
// }

// // convert hue, saturation and brightness to RGB
// // hue: 0-360, saturation: 0-100, brightness: 0-100
// // r, g, b: 0-255
// void HSBtoRGB(int hue, int saturation, int brightness, int &r, int &g, int &b) {
//   int h = round(hue / 60.0);
//   int s = round(saturation / 100.0 * 255);
//   int v = round(brightness / 100.0 * 255);

//   int c = s * v / 255;
//   int x = c * (1 - abs((h % 2) - 1));
//   int m = v - c;

//   int r1, g1, b1;
//   if (h == 0) {
//     r1 = c;
//     g1 = x;
//     b1 = 0;
//   } else if (h == 1) {
//     r1 = x;
//     g1 = c;
//     b1 = 0;
//   } else if (h == 2) {
//     r1 = 0;
//     g1 = c;
//     b1 = x;
//   } else if (h == 3) {
//     r1 = 0;
//     g1 = x;
//     b1 = c;
//   } else if (h == 4) {
//     r1 = x;
//     g1 = 0;
//     b1 = c;
//   } else if (h == 5) {
//     r1 = c;
//     g1 = 0;
//     b1 = x;
//   }

//   r = r1 + m;
//   g = g1 + m;
//   b = b1 + m;
// }
