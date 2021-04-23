// We collect each device caracteristics and store them in BeaconData
enum states{ MOVE, BTN, MOVE_AND_BTN, NOTHING };

typedef struct {
  char address[18];   // 67:f1:d2:04:cd:5d
  int rssi=0;
  int txPower=0; 
  uint8_t batteryLevel=0;//Beacon Battery
  int8_t temperature=0; //Beacon Temperature
  uint8_t number=0; // Indice of the beacon found
  uint8_t timeSinceLastClick=255; // number of seconds since the last click on button (Saturation value)
  uint8_t moveFlag=0; // Bit flag to know beacon movement
  uint8_t state = NOTHING;
} BeaconData;

bool beaconFound = false;
bool locationPacketFound =false;
//uint8_t locationPacketCount =0;
BeaconData buffer[100];    // Buffer to store found device data

//Kontakt identifier for parsing payload Data [hexadecimal number]
char * biotName = "42494f54"; //BIOT in heaxadecimal 
// To improve dev
//char * payloadKontaktID = "6afe02"; 
//char * payloadKontaktTLMID = "6afe030902";
//char * payloadKontaktButtonID = "6afe03030d";
//char * payloadKontaktLocationID = "6afe05";


class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  public:
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      //extern uint8_t bufferIndex;
      extern BeaconData buffer[];

      if (nb_detected >= 100) {
        return;
      }

      // Bool initialization
      beaconFound = false;
      locationPacketFound =false;
      
      //Packet acquisition
      uint8_t *payLoad = advertisedDevice.getPayload(); //Get the entire packet 
      char *payLoadHex = BLEUtils::buildHexData(nullptr, advertisedDevice.getPayload(), advertisedDevice.getPayloadLength()); //Convert the packet in hexadecimal string
        
      //Biot Beacon check
      char* biotFound = strstr(payLoadHex,biotName); 

      //Packet location check 
      if(payLoad[5] ==0x6A && payLoad[6]==0xFE && payLoad[7] ==0x05)
      {
        locationPacketFound = true; 
      }
      
      // Filter: BIot packet and location packet
      if(biotFound !=NULL || locationPacketFound == true)
      {

            //Copy the actual mac adress to analyze it (fort better reading)
            char macAdressReceived[18] ;
            strcpy(macAdressReceived, advertisedDevice.getAddress().toString().c_str());
              
            //First beacon find 
            if(nb_detected ==0)
            {
              strcpy (buffer[nb_detected].address, macAdressReceived);
              buffer[nb_detected].number = nb_detected;
              nb_detected++;
            }
            // Other beacon to store
            else if (nb_detected !=0)
            {   
                      
                  for(uint8_t idx = 0; idx<nb_detected; idx ++)
                   {                      
                      if (strcmp(buffer[idx].address,macAdressReceived)==0) //if the beacon already exists
                      {
                        beaconFound =true;
                        parsePayload(payLoad,idx); //Parse correctly the data
                        break;
                      }
   
                   }
               
                  if (beaconFound == false)
                  {
                    strcpy (buffer[nb_detected].address, macAdressReceived); // Store a new beacon 
                    buffer[nb_detected].number = nb_detected;
                    parsePayload(payLoad,nb_detected); //Parse correctly the data
                    nb_detected++;
                  }


                   // Get RSSI
                  if (advertisedDevice.haveRSSI()) 
                  {
                    buffer[nb_detected].rssi = advertisedDevice.getRSSI();
                  } 
                  else 
                  {
                    buffer[nb_detected].rssi =  0;
                  }
              
              }

      }
 
  }
};

void ScanBeacons() {
  sanityCheck = false; //not use at this moment
  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  MyAdvertisedDeviceCallbacks cb; //define callback
  pBLEScan->setAdvertisedDeviceCallbacks(&cb);
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  BLEScanResults foundDevices = pBLEScan->start(beaconScanTime);
  BLEDevice::getScan()->stop(); // Stop BLE

  //Prints to show in Serial
  Serial.print("\n\n");
  printLocalTime();
  Serial.print("B-IoT devices found: ");
  Serial.println(nb_detected);

  for (uint8_t i = 0; i < nb_detected; i++) {
    
    //State beacon assignement
    if (buffer[i].moveFlag ==1 && buffer[i].timeSinceLastClick<=10)
    {
     buffer[i].state = MOVE_AND_BTN;
    }
    else if(buffer[i].moveFlag == 1)
    {
      buffer[i].state = MOVE;
    }
    else if( buffer[i].timeSinceLastClick <= 10)
    {
      buffer[i].state = BTN;
    }
    else
    {
      buffer[i].state =NOTHING;
    }
    

    //Prints to show in Serial (for dev check)
    Serial.print("Number: ");
    Serial.println(buffer[i].number);
    Serial.print("Mac: ");
    Serial.println(buffer[i].address);
    Serial.print("RSSI: ");
    Serial.println(buffer[i].rssi);
    Serial.print("TX Power: ");
    Serial.println(buffer[i].txPower);
    Serial.print("Battery Level: ");
    Serial.print(buffer[i].batteryLevel);
    Serial.println("%");
    Serial.print("Temperature: ");
    Serial.print(buffer[i].temperature);
    Serial.println("°C");
    Serial.print("Time Button: ");
    Serial.println(buffer[i].timeSinceLastClick);
    Serial.print("Move flag: ");
    Serial.println(buffer[i].moveFlag);
    Serial.print("State:  ");
    Serial.println(buffer[i].state);
    
    Serial.println("---------------------");
  }
  delay(500);
  Serial.println("\nScan done!\n");

  delay(500);
}
