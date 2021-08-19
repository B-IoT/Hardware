/*Version: V1.2 CHUV demo the most stable version to test location engine
   Caractéristics:
   Treatment beacon: V1
   ESP synchronization: YES
   JSON: 1 by 1
*/


//Librairies
#include <ArduinoJson.h>
#include <WiFi.h>
#include "time.h"

//Bluetooth librairies
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEBeacon.h>

#include "MQTT_lib_init_struct.h"

#include "variables.h"

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
  Serial.println("Begin relay setup");
  Serial.println("BEGIN hardcoded information---------------------------------------------");
  Serial.printf("Relay with the ID: %s\n", relayID);
  Serial.printf("company: %s\n", company);
  Serial.printf("mqttUser: %s\n", mqttUser);
  Serial.printf("mqttPassword: %s\n", mqttPassword);
  Serial.printf("hardcoded ssid: %s\n", hardSSID);
  Serial.printf("hardcoded password: %s\n", hardPassword);
  Serial.printf("ServerUri: %s\n", mqttServerUri);
  Serial.println("END hardcoded information---------------------------------------------");

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
  init_and_connect_MQTT();

}

void loop() {

  struct tm timeinfo;

  //Checking Wifi
  if (WiFi.status() != WL_CONNECTED) {
    connect_wifi();
  }

  //Checking MQTT if Wifi connected
  /*if(WiFi.status() == WL_CONNECTED && !client.connected()) {
    connect_MQTT();
    }*/

  //Looks for MQTT messages to read (params to update)
  //client.loop();

  //Scan the beacons around
  ScanBeacons();
  getLocalTime(&timeinfo);
  char timeSec[3];
  strftime(timeSec, 3, "%S", &timeinfo);

  while (String(timeSec).toInt() % 3 != 0) {
    getLocalTime(&timeinfo);
    strftime(timeSec, 3, "%S", &timeinfo);
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
