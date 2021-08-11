// We collect each device caracteristics and store them in BeaconData
typedef struct {
  char id[25]; //Device Name
  char address[18]; // 67:f1:d2:04:cd:5d (mac address)
  int rssi;
  int txPower;
  uint8_t batteryLevel = 80; // Beacon Battery
  int8_t temperature = 22;  // Beacon Temperature
  uint8_t state = 0; // State of the beacon move
  
} BeaconData;

uint8_t bufferIndex = 0;  // Found devices counter
BeaconData buffer[50];    // Buffer to store found devices data

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
  char whiteList[12][18] = // 10 is the length of the longest string + 1 ( for the '\0' at the end ) DEPRECATED
  {
    "d4:80:72:2f:95:aa",
    "e0:51:30:48:16:e5",
    "f5:a8:ef:56:d7:c0",
    "e2:30:d3:c9:fb:66",
    "d1:cf:96:e7:33:ed",
    "f9:b3:b2:3d:53:a5",
    "c6:c6:f1:4d:24:13",
    "d9:ad:89:d1:a7:75",
    "f0:15:b5:dd:24:38",
    "ca:36:8b:4a:a6:1c",
    "ff:d8:05:64:9a:9c",
    "d5:b1:89:8b:b8:c5"
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
  //printLocalTime();
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
     Serial.print("Batterie: ");
    Serial.println(buffer[i].batteryLevel);
    Serial.print("Temperature: ");
    Serial.println(buffer[i].temperature);
    Serial.print("state: ");
    Serial.println(buffer[i].state);
    Serial.println("---------------------");
  }
  bufferIndex = 0;
  Serial.println("\nScan done!\n");
}
