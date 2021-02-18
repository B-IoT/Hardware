void connect_wifi() {

  Serial.print("\nChecking WiFi..");
  
  while (WiFi.status() != WL_CONNECTED) { //Keeps trying to connect until it's ok
    
    Serial.print(".");
    WiFi.begin(mqttSSID, mqttPasswordWIFI); //tries to connect with the MQTT received credentials
    digitalWrite (ledPin, true); //Led ON
    delay(500); // wait 500ms

    //if still not connected
    if (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(hardSSID, hardPassword); //tries to connect with the default credentials
      digitalWrite (ledPin, false);//Led OFF
      delay(500); // wait 500ms
    }
    
  }
  Serial.println(" Connected !");
}
