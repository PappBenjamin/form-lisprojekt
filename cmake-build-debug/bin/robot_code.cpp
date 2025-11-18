#include <Servo.h>

// Pin Definitions
#define LED_PIN 13
#define MOTOR_LEFT_FORWARD 5
#define MOTOR_LEFT_BACKWARD 6
#define MOTOR_RIGHT_FORWARD 10
#define MOTOR_RIGHT_BACKWARD 11
#define DISTANCE_SENSOR_PIN 14
#define LIGHT_SENSOR_PIN 15

// Global Variables
Servo servo;

void avoidObstacle() {
  }
    delay(225);
    digitalWrite(11, HIGH);
    digitalWrite(6, LOW);
    digitalWrite(10, LOW);
    digitalWrite(5, HIGH);
    // Turn right: 45 degrees
    digitalWrite(11, LOW);
    digitalWrite(10, LOW);
    digitalWrite(6, LOW);
    digitalWrite(5, LOW);
    // Stop all motors
  if (analogRead(14) < 20) {

}

void setup() {
// Initialize pins
pinMode(13, OUTPUT);
pinMode(5, OUTPUT);
pinMode(6, OUTPUT);
pinMode(10, OUTPUT);
pinMode(11, OUTPUT);
Serial.begin(9600);
Serial.println("Robot initialized");
}

void loop() {
// Robot: myRobot
// Initializing robot systems...
// Move forward: 100 units
digitalWrite(5, HIGH);
digitalWrite(10, HIGH);
digitalWrite(6, LOW);
digitalWrite(11, LOW);
delay(1000);
// Turn left: 90 degrees
digitalWrite(5, LOW);
digitalWrite(10, HIGH);
digitalWrite(6, HIGH);
digitalWrite(11, LOW);
delay(450);
// Move forward: 50 units
digitalWrite(5, HIGH);
digitalWrite(10, HIGH);
digitalWrite(6, LOW);
digitalWrite(11, LOW);
delay(500);
if (analogRead(14) < 30) {
  // Stop all motors
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  // Turn right: 180 degrees
  digitalWrite(5, HIGH);
  digitalWrite(10, LOW);
  digitalWrite(6, LOW);
  digitalWrite(11, HIGH);
  delay(900);
  // Move backward: 20 units
  digitalWrite(5, LOW);
  digitalWrite(10, LOW);
  digitalWrite(6, HIGH);
  digitalWrite(11, HIGH);
  delay(200);
}
for (int i = 0; i < 3; i++) {
}
// Servo arm to angle 90
servo.write(90);
delay(100);
// Motor left speed: 75%
analogWrite(5, 191);
// Motor right speed: 75%
analogWrite(10, 191);
while (analogRead(15) > 50) {
  // Move forward: 10 units
  digitalWrite(5, HIGH);
  digitalWrite(10, HIGH);
  digitalWrite(6, LOW);
  digitalWrite(11, LOW);
  delay(100);
  delay(100);  // Wait 100ms
}
avoidObstacle();  // Call function
Serial.println("Task complete");
}
