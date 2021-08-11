void connect_wifi() {

  //Serial.print("\nChecking WiFi..");

  while (WiFi.status() != WL_CONNECTED) { //Keeps trying to connect until it's ok

    Serial.print(".");
    WiFi.begin(mqttSSID, mqttPasswordWIFI); //tries to connect with the MQTT received credentials
    ledTurquoiseOn(); //Led turquoise
    delay(250); // wait 500ms

    if (WiFi.status() != WL_CONNECTED) {
      //if still not connected
      WiFi.begin(hardSSID, hardPassword); //tries to connect with the default credentials
      ledGreenOn(); //
      delay(250); // wait 500m
    }
  }
  Serial.println(" Connected !");
  ledGreenOn();

}
