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
// - GPS
// - Direct send to another Iridium modem
// - Sleep

// Receiver unit
// - display


long gpsTimeOutThreshold = 60 * 15; 

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
    if(sendOnce){
       char payload[100];
       sprintf(payload, "%f,%s,%f,%s", latitude, latHem, longitude, lonHem);
       Serial.print("Payload:");
       Serial.println(payload);
      // isuQueue(payload);
       delay(1000);
      // isuSend();
       sendOnce = 1; // set to 1 to repeatedly send
  }

  readISU();
  updateSignalStrength();
  Serial.println(sigStrength);
  
  int incomingByte;
  while (HWSERIAL.available() > 0) {    
      incomingByte = HWSERIAL.read();
      Serial.write(incomingByte);
      gps(incomingByte);  // parse incoming GPS data
  }
}


