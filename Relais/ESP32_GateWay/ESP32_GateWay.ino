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
#include <BLEBeacon.h>
//WiFI parameters
const char* hardSSID = "IT_ELS";
const char* hardPassword = "BLIs0urce19";

//MQTT Parameters
const char* mqttServer = "mqtt.b-iot.ch";
const int mqttPort = 1883;
const char* mqttUser = "test22";
const char* mqttPassword = "test22";
const char* relayID = "relay_22";

//Wi-Fi parameters from MQTT
int mqttFloor = 0;
float mqttLatitude = 0;
float mqttLongitude = 0;
char* mqttSSID;
const char* mqttPasswordWIFI;

//Led parameters
const int ledGreen = 22;
const int ledBlue = 23;
const int ledRed = 3;
const int ledPlus = 1;

const int freq = 5000;
const int ledChannelGreen = 0;
const int ledChannelBlue = 1;
const int resolution = 12; 
const int intensiteOn = 3850;
const int intensiteOff = 4095;

//scan parameters
int beaconScanTime = 4; //Scan time must be longer than beacon interval
uint8_t nb_detected = 0; //nb of beacons detected
bool sanityCheck = false;
//Function to fetch time - Used for proto dev but useless IRL


//Client name for the MQTT
WiFiClient espclient;
PubSubClient client(espclient);

void setup() { //Setup - 10s

  //Set up the LED pin - TBM into RGB
  pinMode (ledGreen, OUTPUT);
  pinMode (ledPlus, OUTPUT);
  pinMode (ledRed, OUTPUT);
  pinMode (ledBlue, OUTPUT);  
  
  digitalWrite (ledGreen, HIGH);  // turn off the LED
  digitalWrite (ledRed, HIGH);  // turn off the LED
  digitalWrite (ledBlue, HIGH);  // turn off the LED 
  digitalWrite (ledPlus, HIGH);  // turn off the LED
  
  ledcSetup(ledChannelGreen, freq, resolution);
  ledcSetup(ledChannelBlue, freq, resolution);
  
  ledcAttachPin(ledGreen, ledChannelGreen);
  ledcAttachPin(ledBlue, ledChannelBlue);
  
  ledGreenOn();
  
  //Begin Serial
  Serial.begin(115200);
  delay(250);  

  //Begin Wifi
  WiFi.begin(hardSSID, hardPassword);
  delay(250);

  //Synchronise time
  GetTime();
  delay(250);

  //Begin bluetooth  
  BLEDevice::init("");
  delay(250);

  //Begin MQTT
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);   
}


void loop() {

   //Checking Wifi
  if(WiFi.status() != WL_CONNECTED) {
    connect_wifi();
}

  //Checking MQTT if Wifi connected
  if(WiFi.status() == WL_CONNECTED && !client.connected()){
    connect_MQTT(); 
  }
  //Looks for MQTT messages to read (params to update)
  client.loop();

  //Scan the beacons around
  ScanBeacons();
  //Send an MQTT after the first update
  if (nb_detected > 0 && mqttLatitude!=0 && mqttLongitude!=0){
    send_MQTT();
  }
}
