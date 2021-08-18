/*Version: V3
 * Caractéristics: 
 * Treatment beacon: V1
 * ESP synchronization: YES                  
 * JSON: 1 by 1
 * MQTT over WSS
 */
 

//Librairies
#include <ArduinoJson.h>
#include <WiFi.h>
#include "time.h"
#include "mqtt_client.h"

//Bluetooth librairies
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEBeacon.h>

#include "MQTT_lib_init_struct.h"

#define MAC_ADDRESS_STRING_LENGTH 18 // accounting for the null char
#define MAC_ADDRESS_LENGTH 6
#define WHITELIST_LENGTH 1024

#define MQTT_JSON_SIZE_RECEIVE 14*1024


//WiFI parameters
//const char* hardSSID = "IT_ELS";
//const char* hardPassword = "BLIs0urce19";

const char* hardSSID = "iPhone de Samuel";
const char* hardPassword = "12345678";

//MQTT Parameters
const char* mqttServerUri = "mqtt://mqtt.b-iot.ch:1883";
const char* mqttUser = "testP1";
const char* mqttPassword = "testP1";
const char* relayID = "relay_P1";

uint8_t MQTT_CONNECTED = 0;

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

  //Begin MQTT
  
  esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);


  /*client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);   */
  init_MQTT();
}

void loop() {
   
  struct tm timeinfo;
    
   //Checking Wifi
  if(WiFi.status() != WL_CONNECTED) {
    connect_wifi();
  }

  // check mqtt
 // if(WiFi.status() == WL_CONNECTED && MQTT_CONNECTED == 0) {
 //   connect_MQTT();
 // }
  
  //Looks for MQTT messages to read (params to update)
  /*client.loop();*/

  //Scan the beacons around
 
  //Send to MQTT after the first update
  if (mqttLatitude != 0 && mqttLongitude != 0) {
    /*Print for dev
      Serial.print("send mqqtt at :");
      Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S\n");
      Serial.println(timeSec);*/

    ledRedOn(); //To see the sending
    //send_MQTT();
    delay(100);
    ledOff();
  }
}
