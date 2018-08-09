// SmartBuoyLoRa
// Transmit GPS position only
// 2018 David Mann, Loggerhead Instruments

// To do: 
// - Sleep between transmissions
// - Test collisions

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

#define LED 13
#define BUOY 1
#define HWSERIAL Serial2 // GPS

long gpsTimeout;
long gpsTimeOutThreshold = 10000; // milliseconds between transmissions

float latitude = 0.0;
float longitude = 0.0;
char latHem, lonHem;
int gpsYear = 0, gpsMonth = 1, gpsDay = 1, gpsHour = 0, gpsMinute = 0, gpsSecond = 0;
int goodGPS = 0;

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);

  HWSERIAL.begin(9600);
  pinMode(LED, OUTPUT);
  delay(4000);
  Serial.println("Loggerhead Smart Buoy");
  Serial1.println("Smart Buoy");

  gpsSpewOn();
  gpsTimeout = 0;
}

int counter;
long startTime = millis();
void loop() {

  // get GPS
  int incomingByte;
  
  goodGPS = 0;
  while(!goodGPS | (millis()-startTime<gpsTimeOutThreshold)){
    while (HWSERIAL.available() > 0) {    
        incomingByte = HWSERIAL.read();
        Serial.write(incomingByte);
        gps(incomingByte);  // parse incoming GPS data
    }
  }

  // only send every timeout
  if(millis()-startTime>=gpsTimeOutThreshold){
    Serial1.write('*');
    Serial1.print(BUOY);
    Serial1.print(":");
    Serial.print(BUOY);
    Serial.print(":");
    // only send GPS if good reading; if bad GPS will only send buoy #
    if(goodGPS){  
      Serial1.print(latitude, 4);
      Serial1.print(" ");
      Serial1.print(longitude, 4);
      Serial.print(latitude, 4);
      Serial.print(" ");
      Serial.println(longitude, 4);
    }
    Serial1.println('!');
    Serial1.flush();
    Serial.println('!');
  }
  startTime = millis();
}
