#include "GpioSwitch.h"

#include <Arduino.h>


GpioSwitch::GpioSwitch(uint8_t pin, bool invertLogic)
    :
    _pin(pin),
    _state(UNKNOWN),
    _invertLogic(invertLogic)
{
}

void GpioSwitch::begin()
{
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, _invertLogic ? HIGH : LOW);
    _state = OFF;
}

GpioSwitch::State GpioSwitch::state() const
{
    return _state;
}

void GpioSwitch::turnOn()
{
    if (_state == ON)
    {
        return;
    }
    digitalWrite(_pin, _invertLogic ? LOW : HIGH);
    _state = ON;
}

void GpioSwitch::turnOff()
{
    if (_state == OFF)
    {
        return;
    }
    digitalWrite(_pin, _invertLogic ? HIGH : LOW);
    _state = OFF;
}
