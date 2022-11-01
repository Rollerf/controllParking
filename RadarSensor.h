#include "Arduino.h"
#include <Switches.h>

class RadarSensor
{
private:
    uint8_t _pin, _type;

public:
    RadarSensor(uint8_t pin, uint8_t count = 1);
    void begin();
    bool read();
};