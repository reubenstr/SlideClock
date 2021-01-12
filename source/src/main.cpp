// WORK IN PROGRESS.

// Time: Uses ESP32 built in NTP client to fetch current date and time.

#include <Arduino.h> // ESP32 built-in
#include <Adafruit_NeoPixel.h>
#include "NeoPixelHelper.h" // Local
#include <WiFi.h>           // ESP32 built-in
#include "time.h"           // ESP32 built-in
#include "Digits.h"         // Local
#include "WifiHelper.h"     // Local
#include "msTimer.h"        // Local

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

void UpdateDigitBackgroundLEDs()
{
  static msTimer timer(20);
  static byte wheelPos;

  if (timer.elapsed())
  {
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
  static msTimer timer;
  static bool toggle;

  if (timer.elapsed())
  {
    toggle = !toggle;
    timer.setDelay(toggle ? 100 : 900);
    digitalWrite(PIN_LED_BUILTIN, toggle);    
  }  
}

void UpdateColonLEDs(bool blinkFlag, bool wifiConnectedFlag)
{
}

// Fetch time uses ESP32 built-in RTC and NTP client.
bool GetTime()
{
  struct tm timeinfo;
  int oldSecond = currentSecond;

  if (getLocalTime(&timeinfo))
  {
    currentHour = timeinfo.tm_hour;
    currentMinute = timeinfo.tm_min;
    currentSecond = timeinfo.tm_sec;
  }
  else
  {
    // Error.
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

  digits.AddDigit(PIN_MOTOR_0_A, PIN_MOTOR_0_B, PIN_LIMIT_0_TOP, PIN_LIMIT_0_BOTTOM, 2600);
  //digits.AddDigit(PIN_MOTOR_1_A, PIN_MOTOR_1_B, PIN_LIMIT_1_TOP, PIN_LIMIT_1_BOTTOM, 2);
  //digits.AddDigit(PIN_MOTOR_2_A, PIN_MOTOR_2_B, PIN_LIMIT_2_TOP, PIN_LIMIT_2_BOTTOM, 3);
  //digits.AddDigit(PIN_MOTOR_3_A, PIN_MOTOR_3_B, PIN_LIMIT_3_TOP, PIN_LIMIT_3_BOTTOM, 4);

  digits.Home();

  while (!digits.IsHomed())
  {
    // Wait for digits to home.
    digits.Tick();
  }

  


  /* 

  AddWifiCredential("RedSky", "happyredcat");
  AddWifiCredential("Hermes", "Mlx5ykch");
  ConnectWifi();

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  */
}

void loop()
{
  BlinkOnboardLED();

  UpdateDigitBackgroundLEDs();

  digits.Tick();

  bool wifiConnectedFlag = (WiFi.status() == WL_CONNECTED);

  static int count = 0;
  static unsigned long start = millis();

  if (millis() - start > 5000)
  {
    start = millis();
    count++;

    digits.SetTargetValue(0, count);
  }


  if (!digits.IsInMotion())
  {




  }

  //bool blinkFlag = GetTime();

  //UpdateColonLEDs(blinkFlag, wifiConnectedFlag);
}