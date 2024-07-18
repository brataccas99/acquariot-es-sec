#include "mqtt.h"

const char* mqtt_server = "e38666b028804b8dbda90cdf0e3dd2a4.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* topic = "acquariot";
const char* mqtt_username = "iotsecurity";
const char* mqtt_password = "IOTsecurity99";
String clientId = "ESP8266Client-" + String(random(0xffff), HEX);


WiFiClientSecure espClient;
PubSubClient client(espClient);

void setupMQTT() {
  Serial.println("Setting up MQTT...");

  // Set the root CA certificate for the SSL/TLS connection
  espClient.setCACert(root_ca);

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  Serial.println("MQTT setup completed.");
}

void reconnectMQTT() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect

    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(topic);
      Serial.print("Subscribed to topic: ");
      Serial.println(topic);
      client.publish(topic, "hello world");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      // Detailed error checking
      if (client.state() == -4) {
        Serial.println("MQTT_CONNECTION_TIMEOUT - the server didn't respond within the keep alive time");
      } else if (client.state() == -3) {
        Serial.println("MQTT_CONNECTION_LOST - the network connection was broken");
      } else if (client.state() == -2) {
        Serial.println("MQTT_CONNECT_FAILED - the network connection failed");
      } else if (client.state() == -1) {
        Serial.println("MQTT_DISCONNECTED - the client is disconnected cleanly");
      } else if (client.state() == 0) {
        Serial.println("MQTT_CONNECTED - the client is connected");
      } else if (client.state() == 1) {
        Serial.println("MQTT_CONNECT_BAD_PROTOCOL - the server doesn't support the requested version of MQTT");
      } else if (client.state() == 2) {
        Serial.println("MQTT_CONNECT_BAD_CLIENT_ID - the server rejected the client identifier");
      } else if (client.state() == 3) {
        Serial.println("MQTT_CONNECT_UNAVAILABLE - the server was unable to accept the connection");
      } else if (client.state() == 4) {
        Serial.println("MQTT_CONNECT_BAD_CREDENTIALS - the username/password were rejected");
      } else if (client.state() == 5) {
        Serial.println("MQTT_CONNECT_UNAUTHORIZED - the client was not authorized to connect");
      }

      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void mqttLoop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
  String incomingMessage = "";
  for (int i = 0; i < length; i++) {
    incomingMessage += (char)payload[i];
  }
  Serial.println("Message arrived [" + String(topic) + "]: " + incomingMessage);
}

void publishMessage(const char* topic, char* payload) {
  if (!client.connected()) {
    client.connect(clientId.c_str(), mqtt_username, mqtt_password);
  }
  if (client.publish(topic, payload))
    Serial.println("Message published [" + String(topic) + "]: " + payload);
  else
    Serial.println("errore nell'invio mex mqtt");
}
