#include <WiFi.h>
#include <WebServer.h>
#include "config.h"
#include "motor_control.h"
#include "ultrasonic.h"


// Replace with your network credentials
const char* ssid     = "pi";
const char* password = "12345678";

// Create an instance of the WebServer on port 80
WebServer server(80);

// Motor 1
int motor1Pin1 = IN1; 
int motor1Pin2 = IN2; 
int enable1Pin = 14;

// Motor 2
int motor2Pin1 = IN3; 
int motor2Pin2 = IN4; 
int enable2Pin = 32;

MotorControl motorControl(motor1Pin1,motor1Pin2,motor2Pin1,motor2Pin2);
// Setting PWM properties
const int freq = 30000;
const int resolution = 8;
int dutyCycle = 0;

String valueString = String(0);

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
  motorControl.forward();
  // digitalWrite(motor1Pin1, LOW);
  // digitalWrite(motor1Pin2, HIGH); 
  // digitalWrite(motor2Pin1, LOW);
  // digitalWrite(motor2Pin2, HIGH);
  server.send(200);
}

void handleLeft() {
  Serial.println("Left");
  motorControl.left();
  // digitalWrite(motor1Pin1, LOW); 
  // digitalWrite(motor1Pin2, LOW); 
  // digitalWrite(motor2Pin1, LOW);
  // digitalWrite(motor2Pin2, HIGH);
  server.send(200);
}

void handleStop() {
  Serial.println("Stop");
  motorControl.stop();
  // digitalWrite(motor1Pin1, LOW); 
  // digitalWrite(motor1Pin2, LOW); 
  // digitalWrite(motor2Pin1, LOW);
  // digitalWrite(motor2Pin2, LOW);   
  server.send(200);
}

void handleRight() {
  Serial.println("Right");
  motorControl.right();
  server.send(200);
}

void handleReverse() {
  Serial.println("Reverse");
  // Test hardware directly
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

void setup() {
  Serial.begin(115200);
  ultrasonic_setup();
  // Configure PWM Pins
  ledcSetup(0, freq, resolution); // Channel 0 for enable1Pin
  ledcAttachPin(enable1Pin, 0);
  ledcSetup(1, freq, resolution); // Channel 1 for enable2Pin
  ledcAttachPin(enable2Pin, 1);
    
  // Initialize PWM with 0 duty cycle
  ledcWrite(0, 0); // Channel 0
  ledcWrite(1, 0); // Channel 1
  
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  int attempts = 0;
  // while (WiFi.status() != WL_CONNECTED && attempts < 20) { // Add retry limit
  //   // delay(500);
  //   // Serial.print(".");
  //   // attempts++;
  // }
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
  ultrasonic_setup();
  Serial.println(get_distance());
  delay(1000);
}