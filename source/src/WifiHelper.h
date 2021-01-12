// Helper methods abstracting connecting to WiFi on a ESP32.

#include "Arduino.h"
#include <WiFi.h> // ESP32 built-in
#include "vector"

struct WifiCredentials
{
    String ssid;
    String password;

    WifiCredentials(String s, String p) : ssid(s), password(p) {}
};

std::vector<WifiCredentials> wifiCredentials;

void AddWifiCredential(String s, String p)
{
    wifiCredentials.push_back(WifiCredentials(s, p));
}

// Connects to WiFi using a list of credentials.
bool ConnectWifi(bool supressSerial = false)
{
    int credentialSelect = 0;
    char buf[256];

    if (wifiCredentials.size() == 0)
    {
        return false;
    }

    if (!supressSerial)
        Serial.println("Attempting to connect to WiFi...");

    while (WiFi.status() != WL_CONNECTED)
    {
        int delayCount = 0;
        sprintf(buf, "Using SSID:%s, Password:%s", wifiCredentials[credentialSelect].ssid.c_str(), wifiCredentials[credentialSelect].password.c_str());
        if (!supressSerial)
            Serial.print(buf);

        if (!WiFi.begin(wifiCredentials[credentialSelect].ssid.c_str(), wifiCredentials[credentialSelect].password.c_str()))
        {
            if (!supressSerial)
                Serial.println("Error when attempting to connect to WiFi.");
            return false;
        }

        while (delayCount++ < 20)
        {
            delay(250);
            Serial.print(".");
        }
        Serial.println("");

        credentialSelect++;

        if (credentialSelect > wifiCredentials.size() - 1)
        {
            credentialSelect = 0;
        }
    }

    sprintf(buf, "WiFi connected. IP address: %s", WiFi.localIP().toString().c_str());
    if (!supressSerial)
        Serial.println(buf);

    return true;
}