#include <Arduino.h>

#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>


void wifiConnect(const char* hostName, const char* ssid, const char* password)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    if (MDNS.begin("esp8266")) 
    {
        Serial.println("MDNS responder started");
    }
    else
    {
        Serial.println("Failed to start MDNS responder");
    }
}

void wifiOnUpdate()
{
    MDNS.update();
}
