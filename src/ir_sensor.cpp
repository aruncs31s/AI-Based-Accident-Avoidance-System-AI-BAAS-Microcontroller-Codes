#include "ir_sensor.h"
#include <Arduino.h>
IR_Sensor::IR_Sensor(uint8_t pin) {
    _pin = pin;
    pinMode(_pin, INPUT);
}

bool IR_Sensor::is_lane() {
    return digitalRead(_pin);
}