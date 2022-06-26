// From https://randomnerdtutorials.com/esp32-mqtt-publish-subscribe-arduino-ide

#include <WiFi.h>
#include "PubSubClient.h"
#include "define.h"

            ///////Global Variables///////////
//////Wifi
const char* SSID = "Wokwi-GUEST";
const char* PWD = "";
//////MQTT Broker
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient); 
const char *mqttServer = "tom.uib.es";
int mqttPort = 1883;
const char topicpubled[] = "esp32statusled";
const char topicsubled[] = "esp32actionled";
char clientId[50];
////////timmer
unsigned long timeroffled = 0 ;
unsigned long timeronled = 0 ;


          ///////////Auxiliar Functions////////
////WiFi
void connectToWiFi() {
  Serial.print("Connectiog to ");
 
  WiFi.begin(SSID, PWD);
  Serial.println(SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print(F("Connected."));
}
////////MQTT Broker
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print(F("Message:"));
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
}

void setupMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);
  // set the callback function
  mqttClient.setCallback(callback);
}

void reconnect() {
  Serial.println(F("Connecting to MQTT Broker..."));
  while (!mqttClient.connected()) {
      Serial.println(F("Reconnecting to MQTT Broker.."));
      String clientId = "ESP32Client-";
      clientId += String(random(0xffff), HEX);
      
      if (mqttClient.connect(clientId.c_str())) {
        Serial.println(F("Connected."));
        // subscribe to topic
        mqttClient.subscribe(topicsubled);
      } 
  }
}

        /////////////Setup////////
void setup() {
  Serial.begin(9600);
  connectToWiFi();
  setupMQTT();
}


      ////////Main////
void loop() {
  if(WiFi.status() != WL_CONNECTED){
    connectToWiFi();
    if(!mqttClient.connected()){
      reconnect();
    }
  }else if(!mqttClient.connected()){
      reconnect();
  }
  mqttClient.loop();
  long now = millis();
  if(millis() - timeronled>def_timeronled){
    mqttClient.publish(topicpubled,"on");
  }
  if(millis() - timeroffled>def_timeroffled){
     mqttClient.publish(topicpubled,"off");
  }
}