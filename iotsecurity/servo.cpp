#include <ESP32Servo.h>
#include "servo.h"

// Servo pin
const int servoPin = 15;
Servo MyServo;

void setupServo() {
  MyServo.attach(servoPin);
}

void mangiare() {
  Serial.println("cibo servito");
  MyServo.write(90);
  delay(1000);
  MyServo.write(9);
}
