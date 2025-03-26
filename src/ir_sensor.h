#include <stdint.h>
#include <Arduino.h> // Add this include

class IR_Sensor {
  public:
    bool is_lane();
    IR_Sensor(uint8_t pin);
  private:
    uint8_t _pin;
    bool _status; // Change type to bool for clarity
};


