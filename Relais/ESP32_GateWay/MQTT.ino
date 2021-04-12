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
    //}
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


  //const char* mac = doc["beacon"]["mac"]; //deprecated
  //long txPower = doc["beacon"]["txPower"]; //deprecated
  //bool ledStatus = doc["ledStatus"];
  //digitalWrite (ledPin, ledStatus); //puts the led in the chosen status by MQTT

  /*Serial.println(mac);
    Serial.println(txPower);
    Serial.println(ledStatus);
    Serial.println("\n--------\n");*/

}

void send_MQTT() {

  //Creating the Json document to send
  DynamicJsonDocument doc(500);                                                                                          //TO INCREASE
  doc["relayID"] = relayID; //title of the Json head is the relayID

  //First array is the RSSI of all devices Json/relayID/rssi
  JsonArray rssi = doc.createNestedArray("rssi");
  for (uint8_t i = 0; i < nb_detected; i++) {
    rssi.add(buffer[i].rssi);
  }

  //Second array is the mac of all devices Json/relayID/mac
  JsonArray mac = doc.createNestedArray("mac");
  for (uint8_t i = 0; i < nb_detected; i++) {
    mac.add(buffer[i].address);
  }

  doc["latitude"] = serialized(String(mqttLatitude, 6)); //latitude of the Json
  doc["longitude"] = serialized(String(mqttLongitude, 6)); //longitude of the Json
  doc["floor"] = mqttFloor;

  //Sending the Json
  char buffer[400];
  serializeJson(doc, buffer);
  client.publish("incoming.update", buffer);
  Serial.println(buffer);
  Serial.println("Sent Json on incoming.update");
  Serial.println("\n---------------------");

}
