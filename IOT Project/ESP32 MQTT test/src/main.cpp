
// From https://randomnerdtutorials.com/esp32-mqtt-publish-subscribe-arduino-ide

#include <WiFi.h>
#include "PubSubClient.h"
                  /////////Global variables////////////////////////////
///Wifi
const char *SSID = "Wokwi-GUEST";
const char *PWD = "";
//// timmer
unsigned long last_timeon = 0;
unsigned long last_timeof = 0;
/////MQTT
// MQTT client
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient); 
PubSubClient client(wifiClient);
const char *mqttServer = "broker.hivemq.com";
int mqttPort = 1883;
const char topicpubled[] = "esp32statusled";
const char topicsubled[] = "esp32actionled";
char clientId[50];



                  ////////////////// Methods////////////////////////
////Wifi
void connectToWiFi() {
  Serial.print(F("Connectiog to "));
  WiFi.begin(SSID, PWD);
  Serial.println(SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(F("."));
    delay(500);
  }
  Serial.print(F("Connected"));
}

////////////MQTT_ Client
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String stMessage;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    stMessage += (char)message[i];
  }
  Serial.println();
}

void setupMQTT() {
  client.setServer(mqttServer, mqttPort);
  // set the callback function
  client.setCallback(callback);
}


void mqttReconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    long r = random(1000);
    sprintf(clientId, "clientId-%ld", r);
    if (client.connect(clientId)) {
      Serial.print(clientId);
      Serial.println(" connected");
      client.subscribe("esp32actionled");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


              ///////////////SETUP////////
void setup() {
  Serial.begin(9600);
  connectToWiFi();
  setupMQTT();
}


        ///////////MAIN///////////777
void loop() {
  if (!client.connected()){
    mqttReconnect();
  }
  client.loop();
  if (millis() - last_timeon > 1000) {
    last_timeon = millis();
    // Send data
    char data[] ="on";
    // Publishing data throgh MQTT
    Serial.println(data);
    client.publish(topicpubled, data);
  }

  if (millis() - last_timeof > 2000) {
    last_timeof = millis();
    // Send data
    char data[] ="off";
    // Publishing data throgh MQTT
    Serial.println(data);
    client.publish(topicpubled, data);
  }

}