// SmartBuoy -- Iridium-GPS module
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
// Transmitter unit
// - Direct send to another Iridium modem
// - hardware sleep pin to Iridium

// Receiver unit
// - display
// - buoy ID: lat, lon

#include <Snooze.h>  //using https://github.com/duff2013/Snooze; uncomment line 62 #define USE_HIBERNATE

SnoozeTimer timer;
// install drivers to a SnoozeBlock
SnoozeBlock config(timer);

long gpsTimeout;
long gpsTimeOutThreshold = 60; 

char sigStrength;
int sendOnce = 1;
float latitude = 0.0;
float longitude = 0.0;
char latHem, lonHem;
int gpsYear = 0, gpsMonth = 1, gpsDay = 1, gpsHour = 0, gpsMinute = 0, gpsSecond = 0;
int goodGPS = 0;


#define HWSERIAL Serial2 //GPS

void setup() {
  Serial.begin(115200);
  Serial1.begin(19200, SERIAL_8N1);
  HWSERIAL.begin(9600);
  
  delay(1000);
  Serial.println("Loggerhead Smart Buoy");

  gpsSpewOn();
  
  Serial1.print("AT\r"); //should get OK
  readISU();
  Serial.println("ATE0");
  Serial1.print("ATE0\r"); //echo off
}

void loop() {
  // get GPS
  int incomingByte;
  
  goodGPS = 0;
  gpsTimeout = 0; // counts once per second when gets GPS sentence
  
  while(!goodGPS){
    while (HWSERIAL.available() > 0) {    
        incomingByte = HWSERIAL.read();
        Serial.write(incomingByte);
        gps(incomingByte);  // parse incoming GPS data
    }

    // Send lat and lon via Iridium if good GPS reading
    if(goodGPS){    
         char payload[100];
         sprintf(payload, "%f,%f", latitude, longitude);
         Serial.print("Payload:");
         Serial.println(payload);
         isuQueue(payload);
         delay(1000);
         isuSend();
    }
    if(gpsTimeout >= gpsTimeOutThreshold) break;
  }

//  readISU();
//  updateSignalStrength();
//  Serial.println(sigStrength);
  

  // Sleep 10 minutes or 60 minutes
  Serial.println("Sleep");
  gpsHibernate();
  iridiumSleep();

//  timer.setTimer(1000 * 600);// milliseconds
//  Snooze.deepSleep(config);
  delay(1000*600);

  /// ..... SLEEPING ......

  gpsWake();
  gpsSpewOn();
  iridiumWake();
}


