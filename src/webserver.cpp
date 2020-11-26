#include "webserver.h"

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <GpioSwitch.h>


static GpioSwitch* _powerSwitch = NULL;

const char* switchStateToString(GpioSwitch::State state)
{
    switch (state)
    {
    case GpioSwitch::ON:
        return "ON";
    case GpioSwitch::OFF:
        return "OFF";
    default:
        return "UNKNOWN";
    }
}

ESP8266WebServer server(80);


void handleRoot()
{
    server.send(200, "text/plain", "hello from esp8266!\r\n");
}

void handleState()
{
    if (server.method() == HTTP_GET)
    {
        server.send(200, "text/plain", switchStateToString(_powerSwitch->state()));
    }
    else if (server.method() == HTTP_POST)
    {
        if (!server.hasArg("state"))
        {
            server.send(400, "txt/plain", "\"state\" argument was not specified\r\n");
            return;
        }

        auto value = server.arg("state");
        if (value.equalsIgnoreCase("ON"))
        {
            _powerSwitch->turnOn();
        }
        else if (value.equalsIgnoreCase("OFF"))
        {
            _powerSwitch->turnOff();
        }
        else
        {
            server.send(400, "txt/plain", "Invalid value for argument \"state\": must be either \"on\" or \"off\".\r\n");
            return;
        }
        server.send(200, "text/plain", "OK");
    }
    else
    {
        server.send(405, "txt/plain", "method not supported\r\n");
    }
}


void webServerStart(GpioSwitch* powerSwitch)
{
    _powerSwitch = powerSwitch;

    server.on("/", handleRoot);
    server.on("/state", handleState);

    server.begin();
    Serial.println("HTTP server started");
}

void webServerUpdate()
{
    server.handleClient();
}