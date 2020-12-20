#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include <WiFi.h>
#include "Digit.h"

const char *ssid = "RedSky";
const char *password = "happyredcat";
const char *host = "data.sparkfun.com";
const char *streamId = "....................";
const char *privateKey = "....................";

#define PIN_LED_BUILTIN 2
#define PIN_LED_STRIP 23

#define PIN_MOTOR_0_A 33
#define PIN_MOTOR_0_B 32
#define PIN_MOTOR_1_A 33
#define PIN_MOTOR_1_B 32
#define PIN_MOTOR_2_A 33
#define PIN_MOTOR_2_B 32
#define PIN_MOTOR_3_A 33
#define PIN_MOTOR_3_B 32

#define PIN_LIMIT_0_TOP 4
#define PIN_LIMIT_0_BOTTOM 14
#define PIN_LIMIT_1_TOP 4
#define PIN_LIMIT_1_BOTTOM 14
#define PIN_LIMIT_2_TOP 4
#define PIN_LIMIT_2_BOTTOM 14
#define PIN_LIMIT_3_TOP 4
#define PIN_LIMIT_3_BOTTOM 14

#define LED_STRIP_NUM_PIXELS (4 * 4) * 4

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_STRIP_NUM_PIXELS, PIN_LED_STRIP, NEO_GRB + NEO_KHZ800);

int currentHour;
int currentMinute;

const int numDigits = 4;
Digit digits[numDigits] = {
    Digit(PIN_MOTOR_0_A, PIN_MOTOR_0_B, PIN_LIMIT_0_TOP, PIN_LIMIT_0_BOTTOM),
    Digit(PIN_MOTOR_1_A, PIN_MOTOR_1_B, PIN_LIMIT_1_TOP, PIN_LIMIT_1_BOTTOM),
    Digit(PIN_MOTOR_2_A, PIN_MOTOR_2_B, PIN_LIMIT_2_TOP, PIN_LIMIT_2_BOTTOM),
    Digit(PIN_MOTOR_3_A, PIN_MOTOR_3_B, PIN_LIMIT_3_TOP, PIN_LIMIT_3_BOTTOM)};

//Digit currentDigits;

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

void setup()
{
  Serial.begin(115200);
  Serial.println("SlideClock startup...");

  pinMode(PIN_LED_BUILTIN, OUTPUT);

  /*
  pinMode(PIN_MOTOR_0_A, OUTPUT);
  pinMode(PIN_MOTOR_0_B, OUTPUT);
  pinMode(PIN_MOTOR_1_A, OUTPUT);
  pinMode(PIN_MOTOR_1_B, OUTPUT);
  pinMode(PIN_MOTOR_2_A, OUTPUT);
  pinMode(PIN_MOTOR_2_B, OUTPUT);
  pinMode(PIN_MOTOR_3_A, OUTPUT);
  pinMode(PIN_MOTOR_3_B, OUTPUT);

  pinMode(PIN_LIMIT_0_TOP, INPUT_PULLUP);
  pinMode(PIN_LIMIT_0_BOTTOM, INPUT_PULLUP);
  pinMode(PIN_LIMIT_1_TOP, INPUT_PULLUP);
  pinMode(PIN_LIMIT_1_BOTTOM, INPUT_PULLUP);
  pinMode(PIN_LIMIT_2_TOP, INPUT_PULLUP);
  pinMode(PIN_LIMIT_2_BOTTOM, INPUT_PULLUP);
  pinMode(PIN_LIMIT_3_TOP, INPUT_PULLUP);
  pinMode(PIN_LIMIT_3_BOTTOM, INPUT_PULLUP);
*/

  strip.begin();
  strip.show();

  for (int i = 0; i < numDigits; i++)
  {
    digits[i].SetTimeBetweenDigits(1000);
  }

  /*
  WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    */
}

void loop()
{

  BlinkOnboardLED();

  strip.setBrightness(127);

  static byte wheelPos;

  wheelPos++;
  for (int i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, Wheel(wheelPos + i * (255 / strip.numPixels())));
  }
  strip.show();

  delay(10);

  //digitalWrite(PIN_MOTOR_4_A, digitalRead(PIN_LIMIT_4_TOP));
  //digitalWrite(PIN_MOTOR_4_B, digitalRead(PIN_LIMIT_4_BOTTOM));

  for (int i = 0; i < numDigits; i++)
  {
    digits[i].Tick();
  }

  //char buf[50];
  //sprintf(buf, "%u %u", digitalRead(PIN_LIMIT_4_TOP), digitalRead(PIN_LIMIT_4_BOTTOM));
  //Serial.println(buf);
  //delay(250);

  /***********************************************************************/
  /***********************************************************************/
  /***********************************************************************/
  /***********************************************************************/
  /*


    delay(5000);
   static int value = 0; 
   ++value;



Serial.print("connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
    }

    // We now create a URI for the request
    String url = "/input/";
    url += streamId;
    url += "?private_key=";
    url += privateKey;
    url += "&value=";
    url += value;

    Serial.print("Requesting URL: ");
    Serial.println(url);

    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }

    // Read all the lines of the reply from server and print them to Serial
    while(client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print(line);
    }

    Serial.println();
    Serial.println("closing connection");

  */
}