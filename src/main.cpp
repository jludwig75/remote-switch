#include <Arduino.h>

#include <GpioSwitch.h>
#include "ota.h"
#include "webserver.h"
#include "wifi.h"

#include "filesystem.h"


const char* ssid = "Caradhras";
const char* password = "Speak friend.";
const char* hostName = "remote-switch";


GpioSwitch powerSwitch(LED_BUILTIN, true);


void setup()
{
    powerSwitch.begin();
    Serial.begin(115200);

    fileSystemSetup();
    wifiConnect(hostName, ssid, password);
    ota_setup();
    webServerStart(&powerSwitch);
}

void loop()
{
    ota_on_loop();
    webServerUpdate();
    wifiOnUpdate();
}