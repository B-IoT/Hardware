//C librairies
#include <sstream>
#include <string>

//Communication librairies
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>


//Time librairie
#include "time.h"
struct tm timeinfo;

//Bluetooth librairies
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>



#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

//Hard-coded parameters
const char* nodeId = "192.168.43.0";
const char* ssid_hard = "test";
const char* password_hard =  "aze123456";
const char* mqtt_server = "192.168.43.196";

//Wi-Fi parameters from MQTT
const char* ssid_MQTT;
const char* password_MQTT;

//led parameters
const int ledPin = 5;
bool ledStatus = true;

//Client name for the MQTT
WiFiClient espClient;
PubSubClient client(espClient);



void setup() { //Setup - 10s

  //Set up the LED pin
  pinMode (ledPin, OUTPUT);  
  digitalWrite (ledPin, ledStatus);
  
  //Begin Serial
  Serial.begin(115200);
  delay(2000);  

  //Begin Wifi
  WiFi.begin(ssid_hard, password_hard);
  delay(2000);

  //Synchronise time
  GetTime();
  delay(2000);

  //Begin bluetooth  
  BLEDevice::init("");
  delay(2000);

  //Begin MQTT
  client.setServer(mqtt_server, 8883);
  client.setCallback(callback);    
}

void loop() {

  if(WiFi.status() != WL_CONNECTED) {
    connect_wifi();}

  if(!client.connected()){
    connect_MQTT();}
  
  client.loop(); //Looks for MQTT messages to read (params to update)
  
  ScanBeacons();

  send_MQTT();
  
  //HTTP_post();

}
