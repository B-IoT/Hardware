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
  char whiteList[24][18] = // 10 is the length of the longest string + 1 ( for the '\0' at the end ) DEPRECATED
  {
    "38:18:4c:2c:3b:74",
    "cf:ae:ce:64:a0:f6",
    "ca:36:8b:4a:a6:1c",
    "f9:b3:b2:3d:53:a5",
    "e3:ba:d7:b6:e3:07",
    "d5:b1:89:8b:b8:c5",
    "d4:80:72:2f:95:aa",
    "f0:15:b5:dd:24:38",
    "ff:d8:05:64:9a:9c",
    "d1:0b:14:b3:18:6a",
    "fc:02:a0:fa:33:19",
    "f5:a8:ef:56:d7:c0",
    "c6:c6:f1:4d:24:13",
    "e0:51:30:48:16:e5",
    "dc:e2:39:31:b9:54",
    "f7:9c:08:9a:42:ed",
    "d1:cf:96:e7:33:ed",
    "ef:86:35:dd:c3:f7",
    "e3:6f:28:36:5a:db",
    "d9:ad:89:d1:a7:75",
    "f1:96:cd:ee:25:bd",
    "e2:30:d3:c9:fb:66",
    "cf:18:14:5d:8a:04",
    "e2:51:e0:31:ee:0e",
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
