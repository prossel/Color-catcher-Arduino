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

#include <Adafruit_NeoPixel.h>

const int ledPin = LED_BUILTIN; // set ledPin to on-board LED
const int buttonPin = 4;        // set buttonPin to digital pin 4

const int stripPin = A7;

Adafruit_NeoPixel strip(12, stripPin, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)

uint8_t rgb_values[3];



BLEService ledService("19B10010-E8F2-537E-4F6C-D104768A1214"); // create service

// create switch characteristic and allow remote device to read and write
BLEByteCharacteristic ledCharacteristic("19B10011-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
// create button characteristic and allow remote device to get notifications
BLEByteCharacteristic buttonCharacteristic("19B10012-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);

// create move characteristic and allow remote device to get notifications
BLEByteCharacteristic moveCharacteristic("19B10013-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);

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
  ledService.addCharacteristic(moveCharacteristic);

  // add the service
  BLE.addService(ledService);

  ledCharacteristic.writeValue(0);
  buttonCharacteristic.writeValue(0);
  moveCharacteristic.writeValue(0); 

  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth® device active, waiting for connections...");

  // NeoPixel strip initialization
  strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();  // Turn OFF all pixels ASAP

  pinMode(stripPin, OUTPUT);
  digitalWrite(stripPin, LOW);

  // Initialize the state machine
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
