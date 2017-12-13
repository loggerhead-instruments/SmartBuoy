
void readISU(){
  char ISU[300] = "";
  char incomingByte;
  int isuCounter = 0;
  int timeout = 10;  // time out in seconds
  unsigned long start = millis();

  while ((millis() - start) < (timeout * 1000)) {
    if(Serial1.available()>0) {
      incomingByte = Serial1.read();
       //ignore line feeds
      if(incomingByte=='\r'){ 
        //Serial.println("Got linefeed");
      }
      else
      {
        ISU[isuCounter] = incomingByte;
        isuCounter += 1;
      }
      if(isuCounter > 1) {
        if(ISU[isuCounter-1]=='K' & ISU[isuCounter-2]=='O') {
          //Serial.println("Got OK");
          while(Serial1.available()){ Serial1.read();} //might be a linefeed to read in
          break;  //break on OK
        }
      }
    } 
  }
  if (isuCounter > 0){
    Serial.println(ISU);
  }
}

void isuQueue(char *moMessage){
  char isuMessage[200];
  sprintf(isuMessage, "AT+SBDWT=%s\r", moMessage);
  Serial.println(isuMessage);
  Serial1.print(isuMessage);
}

void isuSend(){
  Serial.println("sending...");
  Serial1.print("AT+SBDIX\r");
}

void updateSignalStrength(){
// update signal strength
    //Serial1.clear();
    Serial1.print("AT+CSQ\r"); // signal strength

    int i = 0;
    delay(1000);
    char tmp[30];
      while (Serial1.available() > 0 & i<30) {
      tmp[i] = Serial1.read();
      i++;
    }
    Serial.println(tmp);
    sigStrength = tmp[7]; 
}
