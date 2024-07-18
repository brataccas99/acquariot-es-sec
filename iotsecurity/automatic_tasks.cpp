#include "automatic_tasks.h"
#include "RTClib.h"
#include "ds3231.h"
#include "motors.h"
#include "servo.h"
#include "mqtt.h"
#include "thingspeak.h"

// Define the intervals for automatic tasks (in milliseconds)
const unsigned long FEED_INTERVAL = 86400000;       // 24 hours
const unsigned long WATER_CHANGE_INTERVAL = 604800000;  // 7 days
const unsigned long OXYGEN_INTERVAL = 43200000;     // 12 hours

unsigned long lastFeedTimeMillis = 0;
unsigned long lastWaterChangeTimeMillis = 0;
unsigned long lastOxygenTimeMillis = 0;

extern void sendThingSpeakData(int fieldNumber, float message);
extern String getCurrentTimeAsString();

void handleAutomaticTasks() {
  unsigned long currentMillis = millis();

  // Automatic feeding
  if (currentMillis - lastFeedTimeMillis >= FEED_INTERVAL) {
    lastFeedTimeMillis = currentMillis;
    mangiare();
    String currentTime = getCurrentTimeAsString();
    client.publish("acquariot", ("Automatic fish feeding at " + currentTime).c_str());
    sendThingSpeakData(3, 1);
  }

  // Automatic water changing
  if (currentMillis - lastWaterChangeTimeMillis >= WATER_CHANGE_INTERVAL) {
    lastWaterChangeTimeMillis = currentMillis;
    pumpTour();
    String currentTime = getCurrentTimeAsString();
    client.publish("acquariot", ("Automatic water change at " + currentTime).c_str());
    sendThingSpeakData(2, 1);
  }

  // Automatic oxygenation
  if (currentMillis - lastOxygenTimeMillis >= OXYGEN_INTERVAL) {
    lastOxygenTimeMillis = currentMillis;
    ossigeno();
    String currentTime = getCurrentTimeAsString();
    client.publish("acquariot", ("Automatic oxygenation at " + currentTime).c_str());
    sendThingSpeakData(4, 1);
  }
}
