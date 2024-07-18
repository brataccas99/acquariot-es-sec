#ifndef DS3231_H
#define DS3231_H

#include <Wire.h>
#include <RTClib.h>

extern RTC_DS3231 rtc;

void setupDS3231();
float readTemperature();
DateTime checkTime();

#endif
