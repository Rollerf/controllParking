#include "Light.h"
#include "Arduino.h"
#include "Timer.h"

TON *tLightOn;

Light::Light(unsigned char pin)
{
    _pin = pin;
}

void Light::begin()
{
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, HIGH);
    _state = false;
    _manual = false;
    tLightOn = new TON(240000);
}

void Light::turnOn()
{
    _state = true;
    digitalWrite(_pin, LOW);
}

void Light::turnOnWithTimer()
{
    _state = true;
    digitalWrite(_pin, LOW);
    tLightOn->IN(true);
}

void Light::turnOff()
{
    _state = false;
    digitalWrite(_pin, HIGH);
}

bool Light::getState()
{
    return _state;
}

void Light::turnOnManual()
{
    _manual = true;
}

void Light::turnOffManual()
{
    _manual = false;
}

bool Light::isManual()
{
    return _manual;
}

bool Light::isOn()
{
    return _state;
}

void Light::manageLightState()
{
    if (!_manual && tLightOn->IN(true))
    {
        turnOff();
        tLightOn->IN(false);
    }
}