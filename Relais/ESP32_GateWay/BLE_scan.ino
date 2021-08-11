// We collect each device caracteristics and store them in BeaconData
typedef struct {
  char id[25]; //Device Name
  unsigned char address[6]; // 67:f1:d2:04:cd:5d (mac address)
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
      memcpy(buffer[bufferIndex].address, advertisedDevice.getAddress().getNative(), ESP_BD_ADDR_LEN);

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


  unsigned char whiteList[5][6] = 
  {
    { 0xd1, 0x0b, 0x14, 0xb3, 0x18, 0x6a },
    { 0xfc, 0x02, 0xa0, 0xfa, 0x33, 0x19 },
    { 0xe3, 0x6f, 0x28, 0x36, 0x5a, 0xdb },
    { 0xf1, 0x96, 0xcd, 0xee, 0x25, 0xbd }
  };

  //checking whitelist
  nb_detected = 0;
  for (uint8_t i = 0; i <= bufferIndex; i++) {
    for (uint8_t j = 0; j <= sizeof(whiteList) / 18 ; j++) {
        uint8_t eq = 1;
        for (uint8_t k = 0; k < ESP_BD_ADDR_LEN; k++) {
          if(whiteList[j][k] != buffer[i].address[k]) {
            eq = 0;
          }
        }
        if(eq) {
          strcpy(buffer[nb_detected].id, buffer[i].id);
          memcpy(buffer[nb_detected].address, buffer[i].address, ESP_BD_ADDR_LEN);
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
    Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x", buffer[i].address[0], buffer[i].address[1], buffer[i].address[2], buffer[i].address[3], buffer[i].address[4], buffer[i].address[5]);
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
  delay(500);
  Serial.println("\nScan done!\n");
  delay(500);
}
