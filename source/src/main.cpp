// WORK IN PROGRESS.

// Time: Uses ESP32 built in NTP client to fetch current date and time.

#include <Arduino.h> // ESP32 built-in
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>   // ESP32 built-in
#include "time.h"   // ESP32 built-in
#include "Digits.h" // Local

#define PIN_LED_BUILTIN 2
#define PIN_LED_STRIP 5

#define PIN_MOTOR_0_A 33
#define PIN_MOTOR_0_B 32
#define PIN_MOTOR_1_A 27
#define PIN_MOTOR_1_B 26
#define PIN_MOTOR_2_A 18
#define PIN_MOTOR_2_B 17
#define PIN_MOTOR_3_A 16
#define PIN_MOTOR_3_B 4

#define PIN_LIMIT_0_TOP 15
#define PIN_LIMIT_0_BOTTOM 14
#define PIN_LIMIT_1_TOP 13
#define PIN_LIMIT_1_BOTTOM 12
#define PIN_LIMIT_2_TOP 21
#define PIN_LIMIT_2_BOTTOM 19
#define PIN_LIMIT_3_TOP 22
#define PIN_LIMIT_3_BOTTOM 25

#define LED_STRIP_NUM_PIXELS (4 * 4) * 4

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_STRIP_NUM_PIXELS, PIN_LED_STRIP, NEO_GRB + NEO_KHZ800);

int currentHour;
int currentMinute;
int currentSecond;

Digits digits;

// Wifi and NTP
const char *ssid = "RedSky";
const char *password = "happyredcat";
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -18000; // EST
const int daylightOffset_sec = 3600;

/*****************************************************************************/

// Pack color data into 32 bit unsigned int (copied from Neopixel library).
uint32_t Color(uint8_t r, uint8_t g, uint8_t b)
{
  return (uint32_t)((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

// Input a value 0 to 255 to get a color value (of a pseudo-rainbow).
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85)
  {
    return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170)
  {
    WheelPos -= 85;
    return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void UpdateDigitBackgroundLEDs()
{
  static long startMillis;
  static byte wheelPos;

  if (millis() - startMillis > 10)
  {
    startMillis = millis();

    wheelPos++;
    for (int i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, Wheel(wheelPos + i * (255 / strip.numPixels())));
    }
    strip.show();
  }
}

void BlinkOnboardLED()
{
  static unsigned long builtinLedMillis;
  static bool toggle;
  int delay = toggle ? 100 : 900;

  if ((builtinLedMillis + delay) < millis())
  {
    builtinLedMillis = millis();
    toggle = !toggle;
    digitalWrite(PIN_LED_BUILTIN, toggle);
  }
}

void UpdateColonLEDs(bool blinkFlag, bool wifiConnectedFlag)
{
}

// Fetch time uses ESP32 built-in NTP client.
bool GetTime()
{
  static unsigned long startMillis;
  struct tm timeinfo;

  int oldSecond = currentSecond;

  //if (millis() - startMillis > 50)
  //startMillis = millis();

  if (getLocalTime(&timeinfo))
  {
    currentHour = timeinfo.tm_hour;
    currentMinute = timeinfo.tm_min;
    currentSecond = timeinfo.tm_sec;
  }
  else
  {
    return false;
  }

  bool newSecondflag = oldSecond != currentSecond;

  if (newSecondflag)
  {
    char buf[20];
    sprintf(buf, "%02u:%02u:%02u", currentHour, currentMinute, currentSecond);
    Serial.println(buf);
  }

  return newSecondflag;
}

void setup()
{
  Serial.begin(115200);
  Serial.println("SlideClock.");

  pinMode(PIN_LED_BUILTIN, OUTPUT);

  strip.begin();
  strip.setBrightness(16);
  strip.show();

  digits.AddDigit(PIN_MOTOR_0_A, PIN_MOTOR_0_B, PIN_LIMIT_0_TOP, PIN_LIMIT_0_BOTTOM, 500);
  //digits.AddDigit(PIN_MOTOR_1_A, PIN_MOTOR_1_B, PIN_LIMIT_1_TOP, PIN_LIMIT_1_BOTTOM, 2);
  //digits.AddDigit(PIN_MOTOR_2_A, PIN_MOTOR_2_B, PIN_LIMIT_2_TOP, PIN_LIMIT_2_BOTTOM, 3);
  //digits.AddDigit(PIN_MOTOR_3_A, PIN_MOTOR_3_B, PIN_LIMIT_3_TOP, PIN_LIMIT_3_BOTTOM, 4);

  digits.Begin();
  //digits.Home();

  /* */
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop()
{
  BlinkOnboardLED();

  UpdateDigitBackgroundLEDs();

  digits.Tick();

  bool wifiConnectedFlag = (WiFi.status() == WL_CONNECTED);
  
  if (WiFi.status() != WL_CONNECTED)
  {
    // attempt reconnect
  }

  if (!digits.IsInMotion())
  {
    
  }

  bool blinkFlag = GetTime();

  UpdateColonLEDs(blinkFlag, wifiConnectedFlag);
}