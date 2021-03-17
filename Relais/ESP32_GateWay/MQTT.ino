//Json dans lequel on va stocker toutes les données reçues
DynamicJsonDocument doc(2048);

void connect_MQTT() {

  Serial.println("Checking MQTT ...");
  while (!client.connected()) {
    // Attempt to connect
    Serial.print("Connecting to ");
    Serial.println(mqttServer);
    ledBlueOn();
    delay(500);    
    if (client.connect(relayID, mqttUser, mqttPassword)) {
      // Subscribe to channel
      Serial.print("update.parameters state : ");
      Serial.println(client.subscribe("update.parameters"));
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

  Serial.println(" Connected! - Subscribed to each channels");
}


void callback(char* topic, byte* message, unsigned int length) {

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
  StaticJsonDocument<200> doc;                                                                                           //TO INCREASE
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
  
  doc["latitude"] = mqttLatitude; //title of the Json head is the relayID
  doc["longitude"] = mqttLongitude; //title of the Json head is the relayID
  doc["floor"] = mqttFloor;

  //Sending the Json
  char buffer[256];
  serializeJson(doc, buffer);
  client.publish("incoming.update", buffer);
  Serial.println(buffer);
  Serial.println("Sent Json on incoming.update");
  Serial.println("\n---------------------");

}
