
/* 
 *   DISPLAY FUNCTIONS 
 */
 
void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  display.print(":");
  printZero(digits);
  display.print(digits);
}

void printZero(int val){
  if(val<10) display.print('0');
}


void cDisplay(){
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(0,0);
}

void updateDisplay(){
  // show last transmission of each buoy
  cDisplay();

  // if there are messages to display show counter
  if(maxMsg>0){
    display.print(displayMsg + 1);
    display.print("/");
    display.print(maxMsg);
  }

  display.setCursor(50, 0);
  display.print("  S:");
  display.print(sigStrength);

  display.setTextSize(1);
  display.setCursor(0, 20);
  display.println((char *) msgList[displayMsg]);

//  if(displayMsg>0){
//    display.println((char *) msgList[displayMsg-1]); // display previous message
//  }

  display.setCursor(0, BOTTOM);
  display.print("$$$ EN to Update $$$");
  
  display.display();
}

