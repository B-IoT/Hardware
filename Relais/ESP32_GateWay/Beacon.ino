/* Payload treatment: retrieve beacon data*/
/*packetToParse: Packet received to be analyzed
 * bufferIndex: Index of the beacon to store the data
 */
void parsePayload(uint8_t * packetToParse, uint8_t bufferIndex ){
  
          // Packet Kontakt detection ID: 6AFE02
          if(packetToParse[5] ==0x6A && packetToParse[6]==0xFE && packetToParse[7] == 0x02)
          {
            //Serial.println("Kontakt packet");
            //buffer[bufferIndex].packetFlag ="KONTAKT";
            buffer[bufferIndex].batteryLevel = packetToParse[11];
            
            buffer[bufferIndex].txPower = packetToParse[12]; //To convert correctly, in dbm [TO DO ]

          }
          //Packet Kontakt TLM ID: 6AFE030902
          else if (packetToParse[5] ==0x6A && packetToParse[6]==0xFE && packetToParse[7] ==0x03 && packetToParse[8] == 0x09 && packetToParse[9]==0x02)
          {
            //Serial.println("TLM KOntakt packet grouped field");
            //buffer[bufferIndex].packetFlag ="TLM";
            buffer[bufferIndex].batteryLevel = packetToParse[24];
            buffer[bufferIndex].temperature = packetToParse[28];
    
          }
          //Packet Button ID: 6AFE03030D  
          else if (packetToParse[5] ==0x6A && packetToParse[6]==0xFE && packetToParse[7] ==0x03 && packetToParse[8] == 0x03 && packetToParse[9]==0x0D)
          {
            //Serial.println("Button TLM packet");
            //buffer[bufferIndex].packetFlag="BUTTON";
            buffer[bufferIndex].timeSinceLastClick = packetToParse[10];

          }
          //Packet location ID: 6AFE05
          else if(packetToParse[5] ==0x6A && packetToParse[6]==0xFE && packetToParse[7] ==0x05)
          {
            buffer[bufferIndex].moveFlag = packetToParse[11];
          }
}
