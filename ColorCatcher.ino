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

  Libraries to install from Arduino Library Manager:
  - ArduinoBLE by Arduino
  - Adafruit NeoPixel by Adafruit
*/

#include <ArduinoBLE.h>

// Uncomment the following line to use the Arduino Nano 33 BLE Sense Rev 2
// #define REV2

#ifdef REV2
// Arduino Nano 33 BLE Sense Rev 2 has an BMI270 and BMM150
#include "Arduino_BMI270_BMM150.h"
#else
// Original Arduino 33 BLE sense board has an LSM9DS1 IMU
#include <Arduino_LSM9DS1.h>
#endif

#include <Adafruit_NeoPixel.h>

const int ledPin = LED_BUILTIN; // set ledPin to on-board LED
const int buttonPin = 4;        // set buttonPin to digital pin 4

const int stripPin = A7;
#define NB_LEDS 12

Adafruit_NeoPixel strip(NB_LEDS, stripPin, NEO_GRB + NEO_KHZ800);
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

// Battery pin. The battery level is read from this pin
// The max voltage is 3.3V, so the max value is 1023
// The maximum battery level is 4.2V and
// there is a voltage divider that divides the voltage by 1.33
const int batteryPin = A6;
float batteryVoltage = 0;
int batteryLevel = -1; // 0-100

// Palette of 4 colors
struct Color
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
} palette[4] = {
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0}};

// Some stantdard ids from https://www.bluetooth.com/specifications/assigned-numbers/
#define SERVICE_BATTERY "180F"
#define CHARACTERISTIC_BATTERY_LEVEL "2A19"
#define CHARACTERISTIC_USER_DESCRIPTION "2901"

// create a BLE service for the catcher
BLEService ledService("85fa19a3-1000-4cd4-940c-3c038c9aa250"); // create service

// create switch characteristic and allow remote device to read and write
BLEByteCharacteristic ledCharacteristic("85fa19a3-1001-4cd4-940c-3c038c9aa250", BLERead | BLEWrite);

// create button characteristic and allow remote device to get notifications
BLEByteCharacteristic buttonCharacteristic("85fa19a3-1002-4cd4-940c-3c038c9aa250", BLERead | BLENotify);

// create move characteristic and allow remote device to get notifications
BLEByteCharacteristic moveCharacteristic("85fa19a3-1003-4cd4-940c-3c038c9aa250", BLERead | BLENotify);
BLEDescriptor moveDescriptor(CHARACTERISTIC_USER_DESCRIPTION, "Move");

// create move characteristic and allow remote device to get notifications
BLECharacteristic paletteCharacteristic("85fa19a3-1004-4cd4-940c-3c038c9aa250", BLERead | BLEWrite, sizeof(palette));
BLEDescriptor paletteDescriptor(CHARACTERISTIC_USER_DESCRIPTION, "Palette");

// Create a monitoring characteristic (like the serial monitor)
BLEStringCharacteristic monitorCharacteristic("85fa19a3-1099-4cd4-940c-3c038c9aa250", BLERead | BLENotify, 60);
BLEDescriptor buttonDescriptor(CHARACTERISTIC_USER_DESCRIPTION, "Monitor");

// create a BLE service for the battery
BLEService batteryService(SERVICE_BATTERY); // create service

BLEUnsignedCharCharacteristic batteryLevelChar(
    CHARACTERISTIC_BATTERY_LEVEL, // standard 16-bit characteristic UUID
    BLERead | BLENotify);         // remote clients will be able to get notifications if this characteristic changes

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
  ledService.addCharacteristic(paletteCharacteristic);
  ledService.addCharacteristic(monitorCharacteristic);

  // add the descriptor to the characteristic
  moveCharacteristic.addDescriptor(moveDescriptor);
  paletteCharacteristic.addDescriptor(paletteDescriptor);
  monitorCharacteristic.addDescriptor(buttonDescriptor);

  // add the service
  BLE.addService(ledService);

  ledCharacteristic.writeValue(0);
  buttonCharacteristic.writeValue(0);
  moveCharacteristic.writeValue(0);
  paletteCharacteristic.writeValue(palette, 4 * 3);

  // Battery service
  // BLE.setAdvertisedService(batteryService); // do not advertise this service or the device will not be able to connect

  batteryService.addCharacteristic(batteryLevelChar); // add the battery level characteristic
  BLE.addService(batteryService);                     // Add the battery service
  batteryLevelChar.writeValue(batteryLevel);          // set initial value for this characteristic

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

  // read battery level
  readBatteryLevel();

  // Update the state machine
  sm.loop();
}

/*
  Check if the palette has changed and update the palette if it has

  Returns true if the palette has changed

  The palette is a sequence of 4 colors, each color is 3 bytes: R, G, B
*/
bool checkPaletteChanged()
{
  if (paletteCharacteristic.written())
  {
    Serial.println("Palette changed, value length: " + String(paletteCharacteristic.valueLength()) + " bytes");

    // update the palette
    int nBytes = min(sizeof(palette), paletteCharacteristic.valueLength());
    const byte *value = paletteCharacteristic.value();
    for (int i = 0; i < nBytes; i++)
    {
      ((byte *)palette)[i] = value[i];
    }

    // Dump the palette as 0xRRGGBB 0xRRGGBB 0xRRGGBB 0xRRGGBB
    Serial.print("Palette: ");
    for (int i = 0; i < 4; i++)
    {
      char s[] = "0x000000";
      sprintf(s, "0x%02x%02x%02x", palette[i].r, palette[i].g, palette[i].b);
      Serial.print(s);
      Serial.print(" ");
    }
    Serial.println();

    return true;
  }
  return false;
}

void updateRingColorFromPalette()
{
  // Update the ring color. Each color of the palette is assigned to 3 LEDs
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      int led = i * 3 + j;
      strip.setPixelColor(led, strip.Color(palette[i].r, palette[i].g, palette[i].b));
    }
  }
  strip.show();
}

void cycleRingColorFromPalete() {
  const int period = 500;

  // every period, the whole ring takes the next color from the palette
  static int iPalette = 0;
  static unsigned long lastCycle = 0;
  if (millis() > lastCycle + period)
  {
    lastCycle = millis();

    // if next color is 0, 0, 0 restart at index 0
    Color col = palette[iPalette];
    if (col.r == 0 && col.g == 0 && col.b == 0) {
      iPalette = 0;
      col = palette[iPalette];
    }

    // all ring same color
    for (int i = 0; i < NB_LEDS; i++)
    {
      strip.setPixelColor(i, strip.Color(col.r, col.g, col.b));
    }
    strip.show();

    // next color
    iPalette = (iPalette + 1) % 4;
  }
}

void readBatteryLevel()
{
  static float averageVoltage = 0;

  // read the battery level
  int batteryLevelInt = analogRead(batteryPin);

  // calculate the battery voltage
  float batteryVoltage = batteryLevelInt * 3.3 / 1023.0 * 1.33;

  // initial value
  if (averageVoltage < 0.1)
  {
    averageVoltage = batteryVoltage;
  }

  // average the battery voltage over time
  averageVoltage = 0.995 * averageVoltage + 0.005 * batteryVoltage;

  // calculate the battery level without using map function.
  // 3.2V is the minimum voltage, 4.2V is the maximum voltage
  // 0% is the minimum battery level, 100% is the maximum battery level
  int newBatteryLevel = (averageVoltage - 3.2) / (4.2 - 3.2) * 100;
  newBatteryLevel = constrain(newBatteryLevel, 0, 100);

  // update the battery level characteristic if the value has changed enough (seen -20% when passing from 0 leds lit to 12 full white)
  static unsigned long lastUpdateBatteryLevel = 0;
  if (abs(newBatteryLevel - batteryLevelChar.value()) >= 5 ||
      newBatteryLevel != batteryLevelChar.value() && millis() > lastUpdateBatteryLevel + 5000)
  {
    lastUpdateBatteryLevel = millis();

    batteryLevelChar.writeValue((unsigned char)newBatteryLevel);

    Serial.print("Battery voltage: ");
    Serial.print(batteryVoltage);
    Serial.print("V, ");
    Serial.print("Battery average voltage: ");
    Serial.print(averageVoltage);
    Serial.print("V, ");
    Serial.print("Battery level int: ");
    Serial.print(batteryLevelInt);
    Serial.print(", ");
    Serial.print("Battery level: ");
    Serial.print(newBatteryLevel);
    Serial.println("%");
  }

  // update the battery level
  batteryLevel = newBatteryLevel;
}