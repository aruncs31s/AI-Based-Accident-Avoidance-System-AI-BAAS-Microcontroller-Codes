#include "ultrasonic.h"

#include "config.h"

void ultrasonic_setup(){
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
  }
float get_distance(){
    // Send a 10-microsecond pulse to the TRIG_PIN
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long duration = pulseIn(ECHO_PIN, HIGH);
  
    float distance = (duration * 0.034) / 2; // Speed of sound is 0.034 cm/µs
    return distance; // distance in CM
  }