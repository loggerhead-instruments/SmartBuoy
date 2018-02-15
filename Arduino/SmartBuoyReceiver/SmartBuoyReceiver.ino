// SmartBuoyReceiver -- Iridium-GPS module
// Loggerhead Instruments
// copyright 2017
// David Mann

// Wiring
// Yellow Teensy Pin 0 RX1
// Orange Teensy Pin 1 TX1
// Black Ground
// Red 3.3-4.0V LiPo
// Green: No connection
// Brown: No connection


// Iridium ISU module needs to be configured for 3-wire (UART) operation
// Configuration is done using serial connection (e.g. FTDI board)
// Connections: TX-TX, RX-RX, DTR-DTR, CTS-CTS, GND-SG (signal ground)
// AT&D0   (ignore DTR)
// AT&K0   (ignore CTS)
// AT&W0   (store active configuration to memory)
// AT&Y0   (designate as default reset profile)

// Commands must have a carriage return \r, not a line feed
// "AT\r"

// UP/DOWN: Scroll through messages
// ENTER: Mailbox check

// To Do:
// get new messages on button press
// store last 20 messages
// scrolling through messages with button press (up/dn) using LS1

#include "IridiumSBD.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

IridiumSBD modem(Serial1);
#define nMsg 20
int curMsg, maxMsg, displayMsg;


uint8_t rxBuffer[100];
uint8_t msgList[nMsg][100];

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#define BOTTOM 55

// SDA 18
// SCL 19

#define UP_BTN 4
#define DOWN_BTN 3
#define ENTER_BTN 8

int sigStrength;
long startTime;

void setup() {
  Serial.begin(115200);
  Serial1.begin(19200, SERIAL_8N1);
  pinMode(UP_BTN, INPUT);
  pinMode(DOWN_BTN, INPUT);
  pinMode(ENTER_BTN, INPUT);
  pinMode(13, OUTPUT);
  digitalWrite(UP_BTN, HIGH);
  digitalWrite(DOWN_BTN, HIGH);
  digitalWrite(ENTER_BTN, HIGH);
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  //initialize display
  cDisplay();
  display.println("Smart Buoy");
  display.display();
  Serial.println("Loggerhead Smart Buoy");
  delay(2000);

//  Serial1.print("AT\r"); //should get OK
//  readISU();
//  Serial1.print("ATE1\r"); //echo on
//  readISU();
  
  modem.setPowerProfile(IridiumSBD::DEFAULT_POWER_PROFILE);
  int result = modem.begin();
  
  if(result!=0){
    Serial.println("Unable to initialize Iridium");
    Serial.println(result);
    display.println("Startup Error");
    display.display();
    while(1);
  }
 Serial.println("Iridium started");

  modem.getSignalQuality(sigStrength);
  Serial.println(sigStrength);
  updateDisplay();
}

int loopCount = 0;
void loop() {
  loopCount++;
  updateButtons();
  updateDisplay();
  delay(50);

  if(loopCount > 100){
    display.setCursor(60, 0);
    display.println("*");
    display.display();
    modem.getSignalQuality(sigStrength);
    digitalWrite(13, LOW);
    Serial.println(sigStrength);
    loopCount = 0;
  }
}

void getMessages(){
  do
  { 
     size_t bufferSize = sizeof(rxBuffer);
     int status = modem.sendReceiveSBDText(NULL, rxBuffer, bufferSize);
     if (status != ISBD_SUCCESS)
     {
       /* ...process error here... */
       Serial.println("Error getting messages");
       break;
     }
     if (bufferSize == 0) break; // all done!
     /* ...process message in rxBuffer here... */
     Serial.write((char*) rxBuffer);
     memcpy(msgList[curMsg], rxBuffer, sizeof(rxBuffer[0]) * 100);
     displayMsg = curMsg;
     curMsg++;
     if (maxMsg<curMsg) maxMsg = curMsg;
     if(curMsg==nMsg) {
      curMsg = 0; // roll messages
      maxMsg = nMsg;
     }
     updateDisplay();
  }while (modem.getWaitingMessageCount() > 0);
}

bool ISBDCallback(){
  unsigned ledOn = (millis() / 1000) % 2;
  digitalWrite(13, ledOn ? HIGH : LOW); // blink LED every second during send
  
  return true;  // return false if you want to cancel Iridium call
}

void updateButtons(){
  // check for button press

  if(digitalRead(UP_BTN)==0) {
    delay(100);
    Serial.println("Up");
    while(digitalRead(UP_BTN)==0); // wait to let go
    displayMsg++;
  }
  if(digitalRead(DOWN_BTN)==0) {
    delay(100);
    Serial.println("Down");
    while(digitalRead(DOWN_BTN)==0); // wait to let go
    displayMsg--;
  }
  if(digitalRead(ENTER_BTN)==0) {
    delay(100); // require Enter to be held down to get new message
    if(digitalRead(ENTER_BTN)==0){
      Serial.println("Enter");
      cDisplay();
      display.println("Wait...");
      display.display();
      getMessages();
      while(digitalRead(ENTER_BTN)==0); // wait until let go of Enter to exit
    }
  }

  if(displayMsg>=maxMsg) displayMsg = 0; //roll display message
  if(displayMsg<0) displayMsg = maxMsg;
}

