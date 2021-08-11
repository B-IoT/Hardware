void connect_MQTT() {

  //Creating the Json document to send
  DynamicJsonDocument doc(500);
  doc["company"] = "biot";

  //Sending the Json
  char buffer[400];
  serializeJson(doc, buffer);

  Serial.print("Checking MQTT with company id: ");
  Serial.print(buffer);

  while (!client.connected()) {
    //if wifi deconnects after the first check
    if(WiFi.status() != WL_CONNECTED) {
          Serial.println("Wifi exit ...");
          break;
        }

    // Attempt to connect
    Serial.print("Connecting to ");
    Serial.println(mqttServer);
    ledBlueOn();

    delay(500);
    if (client.connect(relayID, mqttUser, mqttPassword, "will", 1, false, buffer)) {
      // Subscribe to channel
      Serial.print("update.parameters state : ");
      Serial.println(client.subscribe("update.parameters"));
     Serial.println(" Connected! - Subscribed to each channels");
      ledBlueOn();
    } else {
      //if cannot connect
      Serial.print(" errorCode= ");
      Serial.println(client.state());
      // Wait 5 seconds before retrying
      ledTurquoiseOn();
      delay(500);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {

  DynamicJsonDocument doc(500);

  //Prints for dev
  Serial.println("\n---------------\n");
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  Serial.print(". Message: ");

  //Char in which the received message is stored
  char messageTemp[200];

  //Converting the receiveid message to char in Json format
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp[i] = (char)message[i];
  }
  //Deserialize the received Json
  DeserializationError error = deserializeJson(doc, messageTemp);

  //Printing error
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  //Updating the parameters
  mqttFloor = doc["floor"];
  mqttLatitude = doc["latitude"];
  mqttLongitude = doc["longitude"];
  mqttSSID = strdup(doc["wifi"]["ssid"]);
  mqttPasswordWIFI = strdup(doc["wifi"]["password"]); 
}

void send_MQTT() {
  
  //Parse the JSON condition 
  /*NOTE: JSON is sending one by one because of a problem occuring from the reception on the backend */
  /*Code in comments allows to send 3 by 3 */
  /*if (nb_detected > maxBeaconToSend) {
    uint8_t startIdx = 0; // Index of the first beacon to send
    uint8_t nbDocToCreate;*/
    uint8_t startIdx = 0;
    uint8_t nbDocToCreate = nb_detected;
    // Compute number of doc to create to send all beacons
    /*if ((nb_detected % maxBeaconToSend) == 0) {
      nbDocToCreate = nb_detected / maxBeaconToSend; //Number of document required to send all beacons
    }
    else {
      nbDocToCreate = nb_detected / maxBeaconToSend + 1; //Number of document required to send all beacons
    }*/
    
    for (uint8_t j = 0; j < nbDocToCreate; j++) {
      uint8_t endIdx = startIdx + maxBeaconToSend; //Index of the last beacon to send 
      
      //Creating the Json document to send
      DynamicJsonDocument doc(600);  //TO INCREASE
      doc["relayID"] = relayID; //Title of the Json head is the relayID

      //Sanity condition to avoid blank data
      if (endIdx > nb_detected) {
        endIdx = nb_detected; 
      }
      
      //Serial print for dev
      /*Serial.printf("startIdx = %d \n",startIdx);
      Serial.printf("endix = %d \n",endIdx);*/
      
      //Fill and send the JSON
      send_JSON(doc, startIdx, endIdx);
      startIdx += maxBeaconToSend; 
    }
  /*} else {
 
    //Creating the Json document to send
    DynamicJsonDocument doc(600);//TO INCREASE
    doc["relayID"] = relayID; //title of the Json head is the relayID
    //Fill and send the JSON
    send_JSON(doc, 0, nb_detected);
  }*/
}

//Goal: Fill the doc and send the JSON doc to the mqtt
//Parameters: doc: the Json document to fill
//            idxStart: index to start the filling
//            idxEnd: index to stop the filling
void send_JSON(DynamicJsonDocument doc, uint8_t idxStart, uint8_t idxEnd) {
    
    
  JsonArray beacons = doc.createNestedArray("beacons"); // Beacon array to send
  
  // Write Beacon data to send
  for (uint8_t i = idxStart; i < idxEnd; i++) {
    DynamicJsonDocument beaconDoc(100); // Json size for on beacon = 78 (measured)

    char macAddressString[18];
    snprintf(macAddressString, MAC_ADDRESS_STRING_LENGTH, "%02x:%02x:%02x:%02x:%02x:%02x", buffer[i].address[0], buffer[i].address[1], buffer[i].address[2], buffer[i].address[3], buffer[i].address[4], buffer[i].address[5]);
    beaconDoc["mac"] = macAddressString;
    beaconDoc["rssi"] = buffer[i].rssi;
    beaconDoc["battery"] = buffer[i].batteryLevel;
    beaconDoc["temperature"] = buffer[i].temperature;
    beaconDoc["status"] = buffer[i].state;
    beacons.add(beaconDoc);
  }
    doc["latitude"] = serialized(String(mqttLatitude, 6)); //latitude of the Json
    doc["longitude"] = serialized(String(mqttLongitude, 6)); //longitude of the Json
    doc["floor"] = mqttFloor;
  
    //Sending the Json
    char buffer[600];
    //Serial.println("Bonjour serial");
    serializeJson(doc, buffer);
    //JSON size for dev
    /*size_t jsonSize =  measureJson(doc);//measure number of caracters inside the json serialized
    Serial.print("JSON SIZE = ");
    Serial.println(jsonSize);*/
    
    client.publish("incoming.update", buffer);
    Serial.println(buffer);
    Serial.println("Sent Json on incoming.update");
    Serial.println("\n---------------------");
}
