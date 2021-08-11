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
//const char* hardSSID = "IT_ELS";
//const char* hardPassword = "BLIs0urce19";
const char* hardSSID = "Wifi-HJU";
const char* hardPassword = "TX-3FqtA1_Cr$nTk";
const char* company = "hju";

//MQTT Parameters
const char* mqttServer = "mqtt.b-iot.ch";
const int mqttPort = 1883;
const char* mqttUser = "test16";
const char* mqttPassword = "test16";
const char* relayID = "relay_16";

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
uint8_t packetScanTime =  2;
int beaconScanTime = 2; //Scan time must be longer than beacon interval
uint8_t nb_detected = 0; //Nb of beacons detected
uint8_t beaconArray = 0; // Nb of beacon to send
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

  //Begin Wifi
  connect_wifi();

  //Synchronise time
  GetTime();

  //Begin MQTT
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  connect_MQTT();

}

void loop() {

  ledTurquoiseOn();

  if (WiFi.status() != WL_CONNECTED) {
    connect_wifi();
  }

  //Checking MQTT if Wifi connected
  if (WiFi.status() == WL_CONNECTED && !client.connected()) {
    connect_MQTT();
  }

  //Looks for MQTT messages to read (params to update)
  client.loop();

  while ((mqttLongitude == 0) && (mqttLatitude == 0)) {
    client.loop();
  }
  
  //Scan the beacons around
  Serial.println("Checking for beacons");

  ScanBeacons();

  struct tm timeinfo;
  getLocalTime(&timeinfo);
  char timeSec[3];
  char timeMin[3];
  strftime(timeSec, 3, "%S", &timeinfo);
  strftime(timeMin, 3, "%M", &timeinfo);


  while (String(timeSec).toInt() % 8 != 0) {
    getLocalTime(&timeinfo);
    strftime(timeSec, 3, "%S", &timeinfo);
    strftime(timeMin, 3, "%M", &timeinfo);
  }

  //Send via MQTT after the first update
  if (String(timeSec).toInt() % 8 == 0 && beaconArray > 0 && mqttLatitude != 0 && mqttLongitude != 0) {
    //Print for dev
    Serial.print("send mqqtt at :");
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S\n");

    ledRedOn(); //To see the sending
    delay(100);
    send_MQTT();
    ledOff();

  }

  ESP.restart();

  if (((String(timeMin).toInt() % 15 == 0) && (String(timeSec).toInt() % 30 == 0)) || (nb_detected == 0)) {
    Serial.print(String(timeMin).toInt());
    Serial.print(" ; ");
    Serial.print(String(timeSec).toInt());
    Serial.println("Rebooting");  
  }
}
