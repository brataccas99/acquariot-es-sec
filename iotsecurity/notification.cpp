#include "notification.h"
#include <Arduino.h>

#define pinBuzzer 22

void setupBuzzer() {
    pinMode(pinBuzzer, OUTPUT);
}

void soundBuzzer(int duration) {
    digitalWrite(pinBuzzer, HIGH);
    delay(duration);
    digitalWrite(pinBuzzer, LOW);
}

void soundBuzzerTwice() {
    soundBuzzer(1000);
    delay(1000); // Delay between beeps
    soundBuzzer(1000);
}
