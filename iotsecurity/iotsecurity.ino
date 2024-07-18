#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <ThingSpeak.h>
#include <UniversalTelegramBot.h>
#include "motors.h"
#include "servo.h"
#include "RTClib.h"
#include "ds3231.h"
#include "mqtt.h"
#include "wifi_setup.h"
#include "automatic_tasks.h"
#include "notification.h"

// Web server object
WebServer server(80);

// Global variables to store last command metrics
String lastFeedTime = "Never";
String lastWaterChangeTime = "Never";
String lastOxygenTime = "Never";
String lastTemperatureTime = "Never";

#define MSG_BUFFER_SIZE 500
char msg[MSG_BUFFER_SIZE];

unsigned long lastTimeBotRan = 0;
const unsigned long BOT_MTBS = 500; // Mean Time Between Scan messages

unsigned long lastTemperatureCheck = 0;
const unsigned long TEMPERATURE_INTERVAL = 600000; // 10 minutes in milliseconds

#define CHAT_ID "379281593"
#define BOT_TOKEN "7301947785:AAE4YMUIJyEmg_oPNIY7CNq-ZcXsGaV5Ve4"

// ThingSpeak settings
unsigned long myChannelNumber = 2601187;
const char* myWriteAPIKey = "2V72MVH5HGJF1RQU";

WiFiClient wifiClient;
WiFiClientSecure telegramClient;
UniversalTelegramBot bot(BOT_TOKEN, telegramClient);

void sendThingSpeakData(int fieldNumber, float value) {
    int x = ThingSpeak.writeField(myChannelNumber, fieldNumber, value, myWriteAPIKey);
    if (x == 200) {
        Serial.println("Channel update successful.");
    } else {
        Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
}

String getCurrentTimeAsString() {
    DateTime now = checkTime();
    char buffer[40];
    snprintf(buffer, sizeof(buffer), "Current time: %02d:%02d:%02d, Day: %d", now.hour(), now.minute(), now.second(), now.day());
    return String(buffer);
}

void handleRoot() {
    server.send(200, "text/plain", "Welcome to the Aquarium Controller");
}

void handleFeedFish() {
    soundBuzzer(3000); // Buzzer sounds for 3 seconds when a command is triggered
    mangiare();
    lastFeedTime = getCurrentTimeAsString();
    
    // Update ThingSpeak with value 1 for feed field
    sendThingSpeakData(3, 1);
    
    publishMessage("acquariot", (char*)("Fish feeding " + lastFeedTime).c_str()); // Publish to MQTT
    server.send(200, "text/plain", "Fish feeding triggered");
    soundBuzzerTwice(); // Buzzer sounds twice when the command is done
}

void handleWaterChange() {
    soundBuzzer(3000); // Buzzer sounds for 3 seconds when a command is triggered
    pumpTour();
    lastWaterChangeTime = getCurrentTimeAsString();
    
    // Update ThingSpeak with value 1 for water_change field
    sendThingSpeakData(2, 1);
    
    publishMessage("acquariot", (char*)("Water change " + lastWaterChangeTime).c_str()); // Publish to MQTT
    server.send(200, "text/plain", "Water change triggered");
    soundBuzzerTwice(); // Buzzer sounds twice when the command is done
}

void handleOxygen() {
    soundBuzzer(3000); // Buzzer sounds for 3 seconds when a command is triggered
    ossigeno();
    lastOxygenTime = getCurrentTimeAsString();
    
    // Update ThingSpeak with value 1 for oxygen field
    sendThingSpeakData(4, 1);
    
    publishMessage("acquariot", (char*)("Oxygenation " + lastOxygenTime).c_str()); // Publish to MQTT
    server.send(200, "text/plain", "Oxygenation triggered");
    soundBuzzerTwice(); // Buzzer sounds twice when the command is done
}

void handleGetMetrics() {
    String response = "Last Feed Time: " + lastFeedTime + "\n"
                      "Last Water Change Time: " + lastWaterChangeTime + "\n"
                      "Last Oxygenation Time: " + lastOxygenTime + "\n"
                      "Last Temperature Time: " + lastTemperatureTime;
    server.send(200, "text/plain", response);
}

void setup() {
    Serial.begin(115200);
    Serial.println("Setup started.");
    setupBuzzer(); // Initialize the buzzer
    // Verify Wi-Fi connection
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected!");
        setupWiFi();
    }

    setupMQTT();
    telegramClient.setCACert(TELEGRAM_CERTIFICATE_ROOT);
    Serial.println("Telegram client initialized");
    bot.sendMessage(CHAT_ID, "Bot started up", "");
    setupDS3231();
    setupMotors();
    setupServo();
    server.on("/", handleRoot);
    server.on("/feed", handleFeedFish);
    server.on("/water_change", handleWaterChange);
    server.on("/oxygen", handleOxygen);
    server.on("/metrics", handleGetMetrics);
    server.begin();
    ThingSpeak.begin(wifiClient);  // Initialize ThingSpeak
    Serial.println("Setup completed.");
}

void loop() {
    mqttLoop();
    server.handleClient();
    handleAutomaticTasks();

    // Check for new messages
    if (millis() - lastTimeBotRan > BOT_MTBS) {
        Serial.println("Checking for new messages");
        int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

        if (numNewMessages > 0) {
            soundBuzzer(1000); // Buzzer sounds for 1 second when a message is received
        }

        while (numNewMessages) {
            for (int i = 0; i < numNewMessages; i++) {
                String text = bot.messages[i].text;
                Serial.print("Received text: ");
                Serial.println(text);
                String from_name = bot.messages[i].from_name;
                String chat_id = String(bot.messages[i].chat_id);

                if (text == "/start") {
                    String welcome = "Welcome, " + from_name + ".\n";
                    welcome += "Use the following commands to control your outputs.\n\n";
                    welcome += "/feed to feed the fishes\n";
                    welcome += "/water_change to change the water\n";
                    welcome += "/oxygen to oxygenate the water\n";
                    welcome += "/metrics to get last command metrics\n";
                    bot.sendMessage(chat_id, welcome, "");
                } else if (text == "/feed") {
                    bot.sendMessage(chat_id, "Feeding the fishes...", "");
                    handleFeedFish();
                } else if (text == "/water_change") {
                    bot.sendMessage(chat_id, "Changing the water...", "");
                    handleWaterChange();
                } else if (text == "/oxygen") {
                    bot.sendMessage(chat_id, "Oxygenating the water...", "");
                    handleOxygen();
                } else if (text == "/metrics") {
                    String metrics = "Last Feed Time: " + lastFeedTime + "\n"
                                     "Last Water Change Time: " + lastWaterChangeTime + "\n"
                                     "Last Oxygenation Time: " + lastOxygenTime + "\n"
                                     "Last Temperature Time: " + lastTemperatureTime;
                    bot.sendMessage(chat_id, metrics, "");
                } else {
                    bot.sendMessage(chat_id, "Unknown command", "");
                }
            }
            numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        }
        lastTimeBotRan = millis();
    }

    // Check and send temperature data 
    if (millis() - lastTemperatureCheck >= TEMPERATURE_INTERVAL) {
        lastTemperatureCheck = millis();
        float temperature = readTemperature();
        snprintf(msg, MSG_BUFFER_SIZE, "Temperature: %.2f", temperature);
        publishMessage("acquariot", (char*)String(msg).c_str()); // Publish to MQTT
        sendThingSpeakData(1, temperature);
    }
}