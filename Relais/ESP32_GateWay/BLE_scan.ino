// We collect each device caracteristics and store them in BeaconData
typedef struct {
  char id[25]; //Device Name
  char address[18]; // 67:f1:d2:04:cd:5d (mac address)
  int rssi;
  int txPower;
  uint8_t batteryLevel = 80; // Beacon Battery
  int8_t temperature = 22;  // Beacon Temperature
  uint8_t state = 0; // State of the beacon move
  float distMeter; 

  
} BeaconData;

uint8_t tx_ref = 7; //TX power value 
int rssi_ref = -59; // RRSI Reference defined by Kontakt 
float distMeter; // Distance Relay-Beacon [m]
float distance_ref; //Real distance of the beacon

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
  char whiteList[1][18] = // 10 is the length of the longest string + 1 ( for the '\0' at the end ) DEPRECATED
  {
    //"cf:ae:ce:64:a0:f6",
    //"d1:0b:14:b3:18:6a",
    //"fc:02:a0:fa:33:19",
   // "e3:6f:28:36:5a:db",
    "f1:96:cd:ee:25:bd",
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
        buffer[nb_detected].distMeter = pow(10,(((float)rssi_ref - (float)buffer[nb_detected].rssi)/(10*(float)tx_ref)));
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
    Serial.printf("Dist meters: %f mètres \n",buffer[i].distMeter);
    
    //Serial.printf("Error dist: %f  mètres \n", distance_ref - buffer[i].distMeter); //Error
    //Serial.print("TX Power: ");
    //Serial.println(buffer[i].txPower);
    Serial.println("---------------------");
  }
  bufferIndex = 0;
  delay(500);
  Serial.println("\nScan done!\n");
  delay(500);
}
