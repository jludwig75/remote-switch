#pragma once


class GpioSwitch;

void webServer_setup(GpioSwitch* powerSwitch);

void webServer_onLoop();