void cDisplay(){
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(0,0);
}

void updateDisplay(){

  display.setTextSize(2);
  display.setCursor(0, 30);
  //display.print("Lat ");
  display.println(latitude);
  
  //display.print("Lon ");
  display.println(longitude);

  display.display();
}

void displaySend(){
  display.setTextSize(2);
  display.setCursor(0,0);
  display.print("Sending");
  display.display();
}

void displaySleep(){
  display.setTextSize(2);
  display.setCursor(0,0);
  display.print("Sleep");
  display.display();
}

void displayPayload(){
   display.setTextSize(1);
   display.setCursor(0, 30);
   display.println(payload);
   display.display(); 
}

