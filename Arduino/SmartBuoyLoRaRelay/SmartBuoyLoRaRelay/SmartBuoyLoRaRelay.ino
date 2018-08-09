// SmartBuoyLoRaRelay
// Retransmits any received messages
// incoming message format for relay: 
//     start with *
//     end with !

// upon relay, add 'r' in front because may receive direct message

// power draw
// ~44 mA in between transmissions (70 mA with GPS on)
// ~450 mA during transmissions (~4 s per receive/transmit)

// 2018 David Mann, Loggerhead Instruments

// To do:
// - GPS

// LoRa Module Parameters (set from EByte RFSetting program)
// Disconnect M0 and M1 jumpers to program module
// UartRate 9600
// Parity: 8N1
// AirRate: 300
// Power: 30 dBm
// FEC: Enable
// Fixed mode: disable
// WOR timing: 250 ms
// IO mode: Push/Pull

// interval between sending GPS
#define gpsTransmitInterval 60000

#define incomingChars 256
char incomingStream[incomingChars];
int incomingPos = 0;
#define HWSERIAL Serial2 // GPS

long gpsTimeout;
long gpsTimeOutThreshold = 10000; // milliseconds between transmissions

float latitude = 0.0;
float longitude = 0.0;
char latHem, lonHem;
int gpsYear = 0, gpsMonth = 1, gpsDay = 1, gpsHour = 0, gpsMinute = 0, gpsSecond = 0;
int goodGPS = 0;

void setup() {
  Serial.begin(115200);  // USB monitor
  Serial1.begin(9600);   // LoRa interface
  HWSERIAL.begin(9600);  // GPS interface
  delay(4000);
  Serial.println("Loggerhead Smart Buoy Relay");
  Serial1.println("Smart Buoy Relay");
}

long startTime = millis();
void loop() {
  // process messages
  while(Serial1.available() >0){
    byte incomingByte = Serial1.read();
    processIncoming(incomingByte);
    Serial.write(incomingByte); // to USB
  }

  // process GPS
   while (HWSERIAL.available() > 0) {    
      gps(HWSERIAL.read());
   }

  // try to send GPS once per minute
  if(millis() - startTime > gpsTransmitInterval){
   if(goodGPS){  
      Serial1.print("<< ");
      Serial1.print(latitude, 4);
      Serial1.print(" ");
      Serial1.println(longitude, 4);
      Serial1.flush();
      Serial.print(latitude, 4);
      Serial.print(" ");
      Serial.println(longitude, 4);
    }
    startTime = millis();
  }
}

int processIncoming(byte incomingByte){
  // * beginning of message
  // ! end of message
  if(incomingByte=='*'){
    incomingStream[0] = 'r'; // add r so know it is relayed message
    incomingPos = 1;
  }

  // end of message, transmit buffer
  if(incomingByte=='!'){
    //send buffer
    Serial1.print(incomingStream);
    Serial.println(incomingStream);
    // erase buffer
    for(int i=0; i<incomingChars; i++){
      incomingStream[i]=NULL;
    }
  }

  // byte in message, add to stream
  incomingStream[incomingPos] = incomingByte;
  incomingPos++;
}

