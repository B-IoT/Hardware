/*Version: V1.2 CHUV demo the most stable version to test location engine
   Caractéristics:
   Treatment beacon: V1
   ESP synchronization: YES
   JSON: 1 by 1
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

//WiFI parameters
//const char* hardSSID = "Wifi-HJU";
//const char* hardPassword = "TX-3FqtA1_Cr$nTk";

//WiFI parameters
const char* hardSSID = "IT_ELS";
const char* hardPassword = "BLIs0urce19";


//MQTT Parameters
const char* mqttServer = "mqtt.b-iot.ch";
const int mqttPort = 1883;
const char* mqttUser = "test21";
const char* mqttPassword = "test21";
const char* relayID = "relay_21";

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
int beaconScanTime = 1; //Scan time must be longer than beacon interval
uint8_t nb_detected = 0; //Nb of beacons detected
uint8_t maxBeaconToSend = 1; //Max nb of beacons to be sent at the same time to the MQTT

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
  connect_wifi();
  
    //Checking Wifi
  if (WiFi.status() != WL_CONNECTED) {
    connect_wifi();
  }
  connect_MQTT();
  
  //Checking MQTT if Wifi connected
  if (WiFi.status() == WL_CONNECTED && !client.connected()) {
    connect_MQTT();
  }
}

void loop() {

  struct tm timeinfo;

  ledTurquoiseOn();
  
  //Checking Wifi
  if (WiFi.status() != WL_CONNECTED) {
    connect_wifi();
  }

  //Checking MQTT if Wifi connected
  else if (WiFi.status() == WL_CONNECTED && !client.connected()) {
    connect_MQTT();
  }
  //Doing the job when WIFI + MQTT = GOOD

  //Looks for MQTT messages to read (params to update)
  client.loop();
  
  Serial.println("Checking for beacons");
  //Scan the beacons around
  ScanBeacons();
  getLocalTime(&timeinfo);
  char timeSec[3];
  strftime(timeSec, 3, "%S", &timeinfo);


  while (String(timeSec).toInt() % 2 != 0) {
    getLocalTime(&timeinfo);
    strftime(timeSec, 3, "%S", &timeinfo);
    //Serial.println(timeSec);
  }
  
  
  //Send to MQTT after the first update
  if (String(timeSec).toInt() % 2 == 0 && nb_detected > 0 && mqttLatitude != 0 && mqttLongitude != 0) {
    //Print for dev
    Serial.print("send mqqtt at :");
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S\n");
    Serial.println(timeSec);

    ledRedOn(); //To see the sending
    send_MQTT();
    delay(100);
    ledOff();

  }
}
