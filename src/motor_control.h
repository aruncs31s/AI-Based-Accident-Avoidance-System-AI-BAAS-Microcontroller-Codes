#include <Arduino.h>

class MotorControl{
  public:
    MotorControl(uint8_t _IN1, uint8_t _IN2, uint8_t _IN3, uint8_t _IN4);
    void forward();
    void backward();
    void left();
    void right();
    void stop();
  private:
    uint8_t _IN1;
    uint8_t _IN2;
    uint8_t _IN3;
    uint8_t _IN4;
    // Need to come fromt the raspberry Pi
    uint8_t speed;
};