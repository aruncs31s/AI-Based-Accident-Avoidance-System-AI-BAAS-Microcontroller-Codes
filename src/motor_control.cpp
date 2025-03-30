#include "motor_control.h"

// Im connecting
/*
0-----0
|     |
|     |
0-----0

Low(0)	Low(0)	Motor OFF
High(1)	Low(0)	Forward
Low(0)	High(1)	Backward
High(1)	High(1)	Motor OFF

| Input 1 | Input 2 | Input 3 | Input 4 | Direction |
| ------- | ------- | ------- | ------- | --------- |
| 0       | 0       | 0       | 0       | Stop      |
| 1       | 0       | 1       | 0       | Front     |
| 0       | 1       | 0       | 1       | Back      |
| 0       | 1       | 1       | 0       | Left      |
| 1       | 0       | 0       | 1       | Right     |

*/

void MotorControl::forward()
{
  Serial.println("FWD");
  digitalWrite(_IN1, HIGH);
  digitalWrite(_IN2, LOW);
  digitalWrite(_IN3, HIGH);
  digitalWrite(_IN4, LOW);
}
void MotorControl::backward(){
  Serial.println("BWD");
  digitalWrite(_IN1, LOW);
  digitalWrite(_IN2, HIGH);
  digitalWrite(_IN3, LOW);
  digitalWrite(_IN4, HIGH);
}
void MotorControl::left()
{
  Serial.println("LFT");
  digitalWrite(_IN1, HIGH);
  digitalWrite(_IN2, LOW);
  digitalWrite(_IN3, LOW);
  digitalWrite(_IN4, HIGH);
  delay(300);
  forward();
}
void MotorControl::right()
{
  Serial.println("RT");

  digitalWrite(_IN1, LOW);
  digitalWrite(_IN2, HIGH);
  digitalWrite(_IN3, HIGH);
  digitalWrite(_IN4, LOW);
  delay(300);
  forward();


}
void MotorControl::stop()
{
  Serial.println("STP");
  digitalWrite(_IN1, LOW);
  digitalWrite(_IN2, LOW);
  digitalWrite(_IN3, LOW);
  digitalWrite(_IN4, LOW);
}
MotorControl::MotorControl(uint8_t _IN1, uint8_t _IN2, uint8_t _IN3, uint8_t _IN4)
{
  this->_IN1 = _IN1;
  this->_IN2 = _IN2;
  this->_IN3 = _IN3;
  this->_IN4 = _IN4;
  pinMode(_IN1, OUTPUT);
  pinMode(_IN2, OUTPUT);
  pinMode(_IN3, OUTPUT);
  pinMode(_IN4, OUTPUT);
}