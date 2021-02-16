/* 
 *  Not used anymore in favor of MQTT
*/

void HTTP_post() {
  
  HTTPClient http; 
  http.begin("https://biot-backend.herokuapp.com/echo");  //Specify destination for HTTP request
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST("{\"data\":\"Hello World\"}");
  delay(250);
  
  if (httpResponseCode > 0) {
    String response = http.getString();                       //Get the response to the request
    Serial.println(httpResponseCode);   //Print return code
    Serial.println(response);           //Print request answer
  } 
  else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }

  http.end();  //Free resources
}
