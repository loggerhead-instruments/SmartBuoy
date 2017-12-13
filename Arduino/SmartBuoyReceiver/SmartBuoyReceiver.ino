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
// - display
// poll for new messages
// - buoy ID: lat, lon


#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#define BOTTOM 55

// SDA 18
// SCL 19

char sigStrength;
int sendOnce = 1;
float latitude = 0.0;
float longitude = 0.0;

char ISU[300];

void setup() {
  Serial.begin(115200);
  Serial1.begin(19200, SERIAL_8N1);
  
  delay(1000);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  //initialize display
  delay(100);
  cDisplay();
  display.println("Smart Buoy");
  display.display();
  Serial.println("Loggerhead Smart Buoy");


  Serial1.print("AT\r"); //should get OK
  readISU();
  Serial.println("ATE0");
  Serial1.print("ATE0\r"); //echo off
}

void loop() {
  readISU();
  updateSignalStrength();
  Serial.println(sigStrength);
  updateDisplay();

  delay(1000);
}


