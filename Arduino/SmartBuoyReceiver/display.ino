
/* DISPLAY FUNCTIONS
 *  
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
  display.print("ID:");
//  display.print(id);
  display.setCursor(20, 0);
  display.print("  S:");
  display.write(sigStrength);

  display.setTextSize(1);
  display.setCursor(0, 10);
  display.print(ISU);
//  display.print("Lat:");
//  display.println(latitude);
//  display.print("Lon:");
//  display.println(longitude);
  
  display.display();
}

