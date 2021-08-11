void connect_wifi() {

  //Serial.print("\nChecking WiFi..");
  
  while (WiFi.status() != WL_CONNECTED) { //Keeps trying to connect until it's ok
    
    //Serial.print(".");
    WiFi.begin(mqttSSID, mqttPasswordWIFI); //tries to connect with the MQTT received credentials
    ledTurquoiseOn(); //Led turquoise
    delay(500); // wait 500ms

    //if still not connected
    if (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(hardSSID, hardPassword); //tries to connect with the default credentials
      ledGreenOn(); // 
      delay(500); // wait 500ms
    }
    
  }
  //Serial.println(" Connected !");
  ledGreenOn();
  
}
