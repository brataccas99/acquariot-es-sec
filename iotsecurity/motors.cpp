#include <Arduino.h>
#include "motors.h"

// Motor pins
const int spurgoPin = 16;
const int freshPin = 21;
const int cicloPin = 19;

void setupMotors() {
  // Set pin modes
  pinMode(spurgoPin, OUTPUT);
  pinMode(freshPin, OUTPUT);
  pinMode(cicloPin, OUTPUT);
}

void pumpTour() {
  Serial.println("carico e pronto all'azione");

  // Turn on the spurgo and fresh pins
  digitalWrite(spurgoPin, HIGH);
  digitalWrite(freshPin, HIGH);

  delay(9000); // Wait for 9 seconds

  // Turn off the spurgo and fresh pins
  digitalWrite(spurgoPin, LOW);
  digitalWrite(freshPin, LOW);
}

void ossigeno() {
  Serial.println("ossigenazione");

  // Turn on the ciclo pin
  digitalWrite(cicloPin, HIGH);

  delay(5000); // Wait for 5 seconds

  // Turn off the ciclo pin
  digitalWrite(cicloPin, LOW);
}
