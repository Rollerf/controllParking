#include "RadarSensor.h"

Switches* sensor;

const bool DIRECT = false;

/*!
 *  @brief  Instantiates a new RadarSensor class
 *  @param  pin
 *          pin number that sensor is connected
 *  @param  count
 *          number of sensors
 */
RadarSensor::RadarSensor(uint8_t pin, uint8_t count) {
  (void)count;
  _pin = pin;
}

void RadarSensor::begin() {
  // set up the pins!
  pinMode(_pin, INPUT);

  sensor = new Switches(40, _pin);
}

bool RadarSensor::read(){
    return sensor->buttonMode(DIRECT);
}
