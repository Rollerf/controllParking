#include "Light.h"
#include "Arduino.h"

Light::Light(unsigned char pin)
{
    _pin = pin;
}

void Light::begin() {
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, HIGH);
  _state = false;
  _mode = "AUTOMATIC";
}

void Light::turnOn()
{
    _state = true;    
    digitalWrite(_pin, LOW);
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

void Light::setMode(String mode)
{
    _mode = mode;
}

String Light::getMode()
{
    return _mode;
}