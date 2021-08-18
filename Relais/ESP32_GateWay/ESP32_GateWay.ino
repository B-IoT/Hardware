/*Version: V3 With temperature, battery and status. Uses Whitelist to check mmac addresses of beacons
 * Caractéristics: 
 * Treatment beacon: V3
 * ESP synchronization: YES                  
 * JSON: 1 by 1
 */
 

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

#define MAC_ADDRESS_STRING_LENGTH 18 // accounting for the null char
#define MAC_ADDRESS_LENGTH 6
#define WHITELIST_LENGTH 1024
#define MQTT_BUFFER_SIZE_RECEIVE 14*1024 // in bytes: 6 * 2 * 1024 + 2 * 1024 (whitelist + 2*1024 for the rest to be sure)
#define MQTT_JSON_SIZE_RECEIVE 14*1024

//WiFI parameters
//const char* hardSSID = "IT_ELS";
//const char* hardPassword = "BLIs0urce19";

const char* hardSSID = "iPhone de Samuel";
const char* hardPassword = "12345678";

//MQTT Parameters
const char* mqttServer = "mqtt.b-iot.ch";
const int mqttPort = 1883;
const char* mqttUser = "testP1";
const char* mqttPassword = "testP1";
const char* relayID = "relay_P1";

//Char in which the received message is stored
char mqttMessageTemp[MQTT_BUFFER_SIZE_RECEIVE];

//WhiteList related variables
uint8_t whiteList[WHITELIST_LENGTH][MAC_ADDRESS_LENGTH]; // White list for the MAC
int whiteListCount = 0; // Counter that keeps track of how many mac addresses are in the white list


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
const int ledChannelRed = 0;
const int resolution = 12; 
const int intensiteOn = 3850;
const int intensiteOff = 4095;

//Scan parameters
int beaconScanTime = 2; //Scan time must be longer than beacon interval
uint8_t nb_detected = 0; //Nb of beacons detected
uint8_t maxBeaconToSendInOnePacket = 1; //Max nb of beacons to be sent at the same time to the MQTT

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
  ledcSetup(ledChannelRed, freq, resolution);
  
  ledcAttachPin(ledGreen, ledChannelGreen);
  ledcAttachPin(ledBlue, ledChannelBlue);
  ledcAttachPin(ledRed, ledChannelRed);
  
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
   
  struct tm timeinfo;
    
   //Checking Wifi
  if(WiFi.status() != WL_CONNECTED) {
    connect_wifi();
  }

  //Checking MQTT if Wifi connected
  if(WiFi.status() == WL_CONNECTED && !client.connected()) {
    connect_MQTT(); 
  }
  
  //Looks for MQTT messages to read (params to update)
  client.loop();

  //Scan the beacons around
  ScanBeacons();
  getLocalTime(&timeinfo);
  char timeSec[3];
  strftime(timeSec, 3, "%S", &timeinfo);

  while(String(timeSec).toInt() % 3 != 0) {
     getLocalTime(&timeinfo);
     strftime(timeSec,3, "%S", &timeinfo);
     delay(10);
     //Serial.println(timeSec);
  }

  //Send to MQTT after the first update
  if (String(timeSec).toInt() % 3 == 0 && nb_detected > 0 && mqttLatitude != 0 && mqttLongitude != 0) {
    /*Print for dev
      Serial.print("send mqqtt at :");
      Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S\n");
      Serial.println(timeSec);*/

    ledRedOn(); //To see the sending
    send_MQTT();
    delay(100);
    ledOff();
  }
}
