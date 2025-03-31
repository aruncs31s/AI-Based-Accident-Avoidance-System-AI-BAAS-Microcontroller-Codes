#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "config.h"
#include "motor_control.h"
#include "ultrasonic.h"
#include <Wire.h>
#include "ir_sensor.h"

// Replace with your network credentials
const char* ssid     = "pi";
const char* password = "12345678";

// Create an instance of the WebServer on port 80
WebServer server(80);

#define LEFT 0
#define RIGHT 1
#define BACK 2
#define FORWARD 3
#define STOP 4

// For Led status for automatic break
bool led_status = 0 ;
uint8_t direction = 0;



uint8_t motor1Pin1 = IN1; 
uint8_t motor1Pin2 = IN2; 

// Motor 2
uint8_t motor2Pin1 = IN3; 
uint8_t motor2Pin2 = IN4; 

uint8_t pwm_pin = PWM_PIN;
uint8_t pwm_pin_2 = PWM_PIN_2;

MotorControl motorControl(motor1Pin1,motor1Pin2,motor2Pin1,motor2Pin2);

IR_Sensor ir_sensor_left(LEFT_IR_PIN); // Pass only left pin
IR_Sensor ir_sensor_right(RIGHT_IR_PIN); // Pass only right pin


// pwm prop ? check if freq is right
const int freq = 30000;
const int resolution = 8; // 0 - 255 
int dutyCycle = 0;

String valueString = String(0);

Ultrasonic ultrasonic(ULTRASONIC_PIN); 


void handleRoot() {
  const char html[] PROGMEM = R"rawliteral(
  <!DOCTYPE HTML><html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="icon" href="data:,">
    <style>
      html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center; }
      .button { -webkit-user-select: none; -moz-user-select: none; -ms-user-select: none; user-select: none; background-color: #4CAF50; border: none; color: white; padding: 12px 28px; text-decoration: none; font-size: 26px; margin: 1px; cursor: pointer; }
      .button2 {background-color: #555555;}
    </style>
    <script>
      function moveForward() { fetch('/forward'); }
      function moveLeft() { fetch('/left'); }
      function stopRobot() { fetch('/stop'); }
      function moveRight() { fetch('/right'); }
      function moveReverse() { fetch('/reverse'); }
    </script>
  </head>
  <body>
    <h1>ESP32 Motor Control</h1>
    <p><button class="button" onclick="moveForward()">FORWARD</button></p>
    <div style="clear: both;">
      <p>
        <button class="button" onclick="moveLeft()">LEFT</button>
        <button class="button button2" onclick="stopRobot()">STOP</button>
        <button class="button" onclick="moveRight()">RIGHT</button>
      </p>
    </div>
    <p><button class="button" onclick="moveReverse()">REVERSE</button></p>
  </body>
  </html>)rawliteral";
  server.send(200, "text/html", html);
}

void handleForward() {
  Serial.println("Forward");
  direction = FORWARD;
  motorControl.forward();
  server.send(200);
  
}

void handleLeft() {
  Serial.println("Left");
  direction = LEFT;
  motorControl.left();
  server.send(200);
}

void handleStop() {
  Serial.println("Stop");
  direction= STOP;
  motorControl.stop();
  server.send(200);
}

void handleRight() {
  Serial.println("Right");
  direction = RIGHT;
  motorControl.right();
  server.send(200);
}

void handleReverse() {
  Serial.println("Reverse");
  direction= BACK;
  ledcWrite(0, 200);
  ledcWrite(1, 200);
  motorControl.backward();
  server.send(200);
}

void handleSpeed() {
  // if (server.hasArg("value")) {
  //   valueString = server.arg("value");
  //   int value = valueString.toInt();
  //   if (value == 0) {
  //     ledcWrite(0, 0); // Channel 0 for enable1Pin
  //     ledcWrite(1, 0); // Channel 1 for enable2Pin
  //     digitalWrite(motor1Pin1, LOW); 
  //     digitalWrite(motor1Pin2, LOW); 
  //     digitalWrite(motor2Pin1, LOW);
  //     digitalWrite(motor2Pin2, LOW);   
  //   } else { 
  //     dutyCycle = map(value, 0, 100, 0, 255); // Correct mapping
  //     ledcWrite(0, dutyCycle); // Channel 0 for enable1Pin
  //     ledcWrite(1, dutyCycle); // Channel 1 for enable2Pin
  //     Serial.println("Motor speed set to " + String(value));
  //   }
  // }
  server.send(200);
}

// Danger led get activated when sensor detects vehicle is crossing the lane
void danger_led(){
  // Check if the vehicle crossed the lane ,  and if one or 2 sensors(ir) crossed the lane turn_on the "LED"
  if(ir_sensor_left.is_lane() && ir_sensor_right.is_lane()){
    digitalWrite(LANE_DETECT_LED_PIN ,LOW);
  }
  else{
    digitalWrite(LANE_DETECT_LED_PIN , HIGH);
  }
}

void automatic_break(){
  switch (ultrasonic.MeasureInCentimeters()) {
    case 0 ... 2: 
    if(direction == BACK){
      ledcWrite(0, 200);
      ledcWrite(1, 200);
    }
    else{
      Serial.println("Obstacle very close, stopping.");
      ledcWrite(0, 0);
      ledcWrite(1, 0);
      digitalWrite(EMERGENCY_STOP_LED_PIN , HIGH);
    
    }
      
      // motorControl.stop();
      
      break;
    case 3 ... 5: 
      if(direction == BACK){
        ledcWrite(0, 200);
        ledcWrite(1, 200);
      
      }
      else{
      Serial.println("Obstacle nearby, reducing speed to half.");

      dutyCycle = 90; 
      ledcWrite(0, dutyCycle);
      ledcWrite(1, dutyCycle);
      led_status = !led_status;
      digitalWrite(EMERGENCY_STOP_LED_PIN , led_status);
      }
      // motorControl.forward();
      break;
    case 6 ... 10: 
    if(direction == BACK){
      ledcWrite(0, 200);
      ledcWrite(1, 200);
    
    }
    else{
      Serial.println("Obstacle at a safe distance, medium speed.");
      dutyCycle = 120; 
      ledcWrite(0, dutyCycle);
      ledcWrite(1, dutyCycle);
      digitalWrite(EMERGENCY_STOP_LED_PIN , LOW);
      // motorControl.forward();  
    }
      

      break;
    default: // Distance greater than 10 cm
      Serial.println("No obstacle detected, full speed.");
      dutyCycle = 220; // Full speed
      ledcWrite(0, dutyCycle);
      ledcWrite(1, dutyCycle);
      digitalWrite(EMERGENCY_STOP_LED_PIN , LOW);
      // motorControl.forward();
      break;
  }
}
void setup() {
  Serial.begin(115200);
  pinMode(LANE_DETECT_LED_PIN, OUTPUT); 
  pinMode(EMERGENCY_STOP_LED_PIN, OUTPUT);

  // (channel , freq , resolution )
  ledcSetup(0, freq, resolution); // Channel 0 for enable1Pin
  ledcAttachPin(pwm_pin, 0);
  ledcSetup(1, freq, resolution); // Channel 1 for enable2Pin
  ledcAttachPin(pwm_pin_2, 1);
    
  // Initialize PWM with 0 duty cycle
  ledcWrite(0, 0); // Channel 0
  ledcWrite(1, 0); // Channel 1
  
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 100) { // Add retry limit
    delay(500);
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("Failed to connect to WiFi. Please check credentials.");
    // while (true); // Halt execution
  }

  // Define routes
  server.on("/", handleRoot);
  server.on("/forward", handleForward);
  server.on("/left", handleLeft);
  server.on("/stop", handleStop);
  server.on("/right", handleRight);
  server.on("/reverse", handleReverse);
  server.on("/speed", handleSpeed);

  // Start the server
  server.begin();
}

void loop() {
  server.handleClient();
  automatic_break();
  danger_led();
  delay(100);
 
}