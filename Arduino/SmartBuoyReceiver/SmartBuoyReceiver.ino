// SmartBuoyReceiver -- Iridium-GPS module
// Loggerhead Instruments
// copyright 2017
// David Mann


// Iridium ISU module needs to be configured for 3-wire (UART) operation
// Configuration is done using serial connection (e.g. FTDI board)
// Connections: TX-TX, RX-RX, DTR-DTR, CTS-CTS, GND-SG (signal ground)
// AT&D0   (ignore DTR)
// AT&K0   (ignore CTS)
// AT&W0   (store active configuration to memory)
// AT&Y0   (designate as default reset profile)

// Commands must have a carriage return \r, not a line feed
// "AT\r"


// To Do:

// Receiver unit
// poll for new messages every x minutes
// get new messages on button press
// - buoy ID: lat, lon

#include "IridiumSBD.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

IridiumSBD modem(Serial1, 10);
uint8_t rxBuffer[100];

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#define BOTTOM 55

// SDA 18
// SCL 19

int sigStrength;
int sendOnce = 1;
float latitude = 0.0;
float longitude = 0.0;

char ISU[300];

void setup() {
  Serial.begin(115200);
 // Serial1.begin(19200, SERIAL_8N1);
 modem.begin();
  
  delay(5000);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  //initialize display
  delay(100);
  cDisplay();
  display.println("Smart Buoy");
  display.display();
  Serial.println("Loggerhead Smart Buoy");
  delay(1000);

  modem.getSignalQuality(sigStrength);
  Serial.println(sigStrength);
  updateDisplay();

//  Serial1.print("AT\r"); //should get OK
//  readISU();
//  Serial.println("ATE0");
//  Serial1.print("ATE0\r"); //echo off
}

void loop() {
  //readISU();
  //updateSignalStrength();
  getMessages();

  for (int x=0; x<60; x++){
    delay(10000); // wait 10 s
    modem.getSignalQuality(sigStrength);
    Serial.println(sigStrength);
    updateDisplay();
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
     updateDisplay();
  }while (modem.getWaitingMessageCount() > 0);
}

bool ISBDCallback(){
  unsigned ledOn = (millis() / 1000) % 2;
  digitalWrite(13, ledOn ? HIGH : LOW); // blink LED every second during send
  return true;
}

