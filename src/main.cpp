#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "credentials.h"
#include <RCSwitch.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define SERIAL_BAUDRATE 115200
#define MQTT_CLIENT_ID "RC-MQTT-Gateway"
#define MQTT_TOPIC_COMMAND "rc/command"
#define JSON_DOCUMENT_SIZE 50

WiFiClient espClient;
PubSubClient mqttClient(espClient);

RCSwitch rcSwitch = RCSwitch();

void wifiSetup();
void mqttCallback(char *topic, byte *payload, unsigned int length);
void mqttReconnect();

void setup()
{

  Serial.begin(SERIAL_BAUDRATE);
  Serial.println();
  Serial.println();

  rcSwitch.enableTransmit(D3);

  wifiSetup();

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
}

void loop()
{
  if (!mqttClient.connected())
  {
    mqttReconnect();
  }
  mqttClient.loop();
}

void mqttReconnect()
{
  while (!mqttClient.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS))
    {
      Serial.println("Connected to mqtt server");
      mqttClient.subscribe(MQTT_TOPIC_COMMAND);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  DynamicJsonDocument doc(JSON_DOCUMENT_SIZE);

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, payload);

  // Test if parsing succeeds.
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  long protocol = doc["protocol"];
  long code = doc["code"];

  if (protocol)
  {
    rcSwitch.setProtocol(protocol);
    Serial.print("rcSwitch Protocol set to: ");
    Serial.print(protocol);
    Serial.println();
  }

  if (code)
  {
    // sending code
    rcSwitch.send(code, 24);
    Serial.print("rcSwitch send code: ");
    Serial.print(code);
    Serial.println();
  }
}

void wifiSetup()
{
  // Set WIFI module to STA mode
  WiFi.mode(WIFI_STA);

  // Connect
  Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Wait
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  // Connected!
  Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}
