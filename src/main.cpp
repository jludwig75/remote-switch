#include <Arduino.h>

#include <GpioSwitch.h>
#include "ota.h"
#include "webserver.h"
#include "wifi.h"

#include "filesystem.h"


const char* ssid = "Caradhras";
const char* password = "Speak friend.";
const char* hostName = "remote-switch";
const char* otaPassword = "Redhorn!1";


GpioSwitch powerSwitch(LED_BUILTIN, true);


void setup()
{
    powerSwitch.begin();
    Serial.begin(115200);

    fileSystem_setup();
    wifi_setup(hostName, ssid, password);
    ota_setup(hostName, otaPassword);
    webServer_setup(&powerSwitch);
}

void loop()
{
    ota_onLoop();
    webServer_onLoop();
    wifi_onLoop();
}