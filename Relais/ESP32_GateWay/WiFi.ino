void connect_wifi() {

  //Serial.print("\nChecking WiFi..");
  int compteur = 0;
  Serial.println("Enable Wifi access point");
  WiFi.enableSTA(true);
  delay(500);
  while (WiFi.status() != WL_CONNECTED && compteur < 100) { //Keeps trying to connect until it's ok

    Serial.println("Attempting to connect to the wifi");

    Serial.print("Hard coded with: ");
    Serial.print(hardSSID);
    Serial.print(" ; ");
    Serial.println(hardPassword);

    WiFi.begin(hardSSID, hardPassword); //tries to connect with the MQTT received credentials
    ledTurquoiseOn(); //Led turquoise
    delay(500); // wait 500ms
    ledOff();

    if (WiFi.status() != WL_CONNECTED) {
      Serial.print("MQTT with: ");
      Serial.print(mqttSSID);
      Serial.print(" ; ");
      Serial.println(mqttPasswordWIFI);
      //if still not connected
      WiFi.begin(mqttSSID, mqttPasswordWIFI); //tries to connect with the default credentials
      ledRedOn(); //
      delay(500); // wait 500ms
      ledOff();
    }
    compteur++;
    Serial.println(compteur);
    
    if (compteur  > 10) {
    compteur = 0;
    Serial.println("Restarting due to WiFi");
    ESP.restart();
  }
  
  }
  
  Serial.println(" Connected !");
  ledGreenOn();

}
