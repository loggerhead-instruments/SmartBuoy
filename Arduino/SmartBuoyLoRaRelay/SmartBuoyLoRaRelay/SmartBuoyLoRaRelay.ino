// SmartBuoyLoRaRelay
// Retransmits any received messages
// incoming message format for relay: 
//     start with *
//     end with !

// upon relay, add 'r' in front because may receive direct message

// 2018 David Mann, Loggerhead Instruments

// To do:
// - GPS
// - measure power draw

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

#define incomingChars 256
char incomingStream[incomingChars];
int incomingPos = 0;
#define LED 13

void setup() {
  Serial.begin(115200);  // USB monitor
  Serial1.begin(9600);   // LoRa interface
  pinMode(LED, OUTPUT);
  delay(4000);
  Serial.println("Loggerhead Smart Buoy Relay");
  Serial1.println("Smart Buoy Relay");

}

void loop() {
  // process messages
  while(Serial1.available() >0){
    byte incomingByte = Serial1.read();
    processIncoming(incomingByte);
    Serial.write(incomingByte); // to USB
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

