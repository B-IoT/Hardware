//Librairies
#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "time.h"

//Bluetooth librairies
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

//WiFI parameters
const char* hardSSID = "test";
const char* hardPassword =  "aze123456";

//MQTT Parameters
const char* mqttServer = "mqtt.b-iot.app";
const int mqttPort = 1883;
const char* mqttUser = "test";
const char* mqttPassword = "test";
const char* relayID = "test";

//Wi-Fi parameters from MQTT
float mqttLatitude = 0;
float mqttLongitude = 0;
char* mqttSSID;
const char* mqttPasswordWIFI;

//Led parameters
const int ledPin = 5;
bool ledStatus = true;

//Client name for the MQTT
WiFiClient espClient;
PubSubClient client(espClient);

void setup() { //Setup - 10s

  //Set up the LED pin - TBM into RGB
  pinMode (ledPin, OUTPUT);  
  digitalWrite (ledPin, ledStatus);
  
  //Begin Serial
  Serial.begin(115200);
  delay(2000);  

  //Begin Wifi
  WiFi.begin(hardSSID, hardPassword);
  delay(2000);

  //Synchronise time
  GetTime();
  delay(2000);

  //Begin bluetooth  
  BLEDevice::init("");
  delay(2000);

  //Begin MQTT
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);    
}


void loop() {

  //Checking Wifi
  if(WiFi.status() != WL_CONNECTED) {
    connect_wifi();}

  //Checking MQTT
  if(!client.connected()){
    connect_MQTT();}

  //Looks for MQTT messages to read (params to update)
  client.loop();
  //Scan the beacons around
  ScanBeacons();

  //Send an MQTT
  send_MQTT();
}
