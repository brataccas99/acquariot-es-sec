#include "ds3231.h"
#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;

void setupDS3231() {
  Wire.begin(4, 2);  // Initialize I2C communication on SDA (GPIO 4) and SCL (GPIO 2)
  Serial.println("I2C initialized.");
  
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  } else {
    Serial.println("RTC initialized");
  }
  
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting the time!");
    // Set the time to the current compile time
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

float readTemperature() {
  return rtc.getTemperature();
}

DateTime checkTime() {
  DateTime now = rtc.now();
  return now;
}
