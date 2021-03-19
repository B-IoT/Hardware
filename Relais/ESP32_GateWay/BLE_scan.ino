// We collect each device caracteristics and store them in BeaconData
typedef struct {
  char id[25] ; //Device Name
  char address[18];   // 67:f1:d2:04:cd:5d
  int rssi;
  int txPower;
} BeaconData;

uint8_t bufferIndex = 0;  // Found devices counter
BeaconData buffer[50];    // Buffer to store found device data

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  public:
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      extern uint8_t bufferIndex;
      extern BeaconData buffer[];

      if (bufferIndex >= 50) {
        return;
      }

      //Name
      if (advertisedDevice.haveName()) {
        strcpy (buffer[bufferIndex].id, advertisedDevice.getName().c_str());
      }
      //RSSI
      if (advertisedDevice.haveRSSI()) {
        buffer[bufferIndex].rssi = advertisedDevice.getRSSI();
      } else {
        buffer[bufferIndex].rssi =  0;
      }
      //MAC Adresse
      strcpy (buffer[bufferIndex].address, advertisedDevice.getAddress().toString().c_str());

      //TX Power
      if (advertisedDevice.haveTXPower()) {
        buffer[bufferIndex].txPower = advertisedDevice.getTXPower();
      }

      //Debug Print
      /*Serial.printf("name: %s \n", advertisedDevice.getName().c_str());
        Serial.printf("MAC: %s \n", advertisedDevice.getAddress().toString().c_str());
        Serial.printf("Manufactuerer Data: %d \n", advertisedDevice.getManufacturerData());
        Serial.printf("RSSI: %d \n", advertisedDevice.getRSSI());
        Serial.printf("TX Power: %d \n", advertisedDevice.getTXPower());*/
      bufferIndex++;
    }
};

void ScanBeacons() {

  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  MyAdvertisedDeviceCallbacks cb; //define callback
  pBLEScan->setAdvertisedDeviceCallbacks(&cb);
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  BLEScanResults foundDevices = pBLEScan->start(beaconScanTime);
  BLEDevice::getScan()->stop(); // Stop BLE

  //Deprecated but kept for dev
  char whiteList[3][18] = // 10 is the length of the longest string + 1 ( for the '\0' at the end ) DEPRECATED
  {
    "e3:ba:d7:b6:e3:07",
    "e2:30:d3:c9:fb:66",
    "e3:ba:d7:b6:e3:07",
  };

  //checking whitelist
  nb_detected = 0;
  for (uint8_t i = 0; i <= bufferIndex; i++) {
    for (uint8_t j = 0; j <= sizeof(whiteList) / 18 ; j++) {
      if (strcmp(buffer[i].address, whiteList[j]) == 0) {
        strcpy(buffer[nb_detected].id, buffer[i].id);
        strcpy(buffer[nb_detected].address, buffer[i].address);
        buffer[nb_detected].rssi = buffer[i].rssi;
        buffer[nb_detected].txPower = buffer[i].txPower;
        nb_detected++;
      }
    }
  }

  //Prints to show in Serial
  Serial.print("\n\n");
  printLocalTime();
  Serial.print("B-IoT devices found: ");
  Serial.println(nb_detected);
  for (uint8_t i = 0; i < nb_detected; i++) {

    Serial.print("Name: ");
    Serial.println(buffer[i].id);
    Serial.print("Mac: ");
    Serial.println(buffer[i].address);
    Serial.print("RSSI: ");
    Serial.println(buffer[i].rssi);
    Serial.print("TX Power: ");
    Serial.println(buffer[i].txPower);
    Serial.println("---------------------");
  }
  bufferIndex = 0;
  delay(500);
  Serial.println("\nScan done!\n");
  delay(500);
}
