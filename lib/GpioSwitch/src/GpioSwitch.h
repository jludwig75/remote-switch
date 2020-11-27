#pragma once

#include <stdint.h>


class GpioSwitch
{
public:
    GpioSwitch(uint8_t pin, bool invertLogic = false);
    void begin();
    enum State
    {
        UNKNOWN,
        ON,
        OFF
    };
    State state() const;
    void turnOn();
    void turnOff();
    void togglePower();
private:
    uint8_t _pin;
    State _state;
    bool _invertLogic;
};