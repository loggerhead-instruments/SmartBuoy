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

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> // modify so calls i2c_t3 not Wire.h
#include <Adafruit_FeatherOLED.h>
#include <TimeLib.h>
#include <EEPROM.h>

#define BUOY 1
#define HWSERIAL Serial2 // GPS

#define LED 17
#define vSense A14

#define displayLine1 0
#define displayLine2 8
#define displayLine3 16
#define displayLine4 25
Adafruit_FeatherOLED display = Adafruit_FeatherOLED();

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
  digitalWrite(LED, HIGH);
  delay(4000);
  Serial.println("Loggerhead Smart Buoy");
  Serial1.println("Smart Buoy");

  
  displayOn();
  cDisplay();
  display.println("Loggerhead");
  display.display();

  gpsSpewOn();
  gpsTimeout = 0;
  digitalWrite(LED, LOW);
}

int counter;
long startTime = millis();
void loop() {

  // get GPS
  int incomingByte;
  
  goodGPS = 0;
  while(millis()-startTime<gpsTimeOutThreshold){
    while (HWSERIAL.available() > 0) {    
        incomingByte = HWSERIAL.read();
        Serial.write(incomingByte);
        gps(incomingByte);  // parse incoming GPS data
    }
  }

  // only send every timeout
  if(millis()-startTime>=gpsTimeOutThreshold){
    digitalWrite(LED, HIGH);
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
  digitalWrite(LED, LOW);
}

float readVoltage(){
   float  voltage = 0;
   float vDivider = 2.1; //when using 3.3 V ref R9 100K
   //float vDivider = 4.5;  // when using 1.2 V ref R9 301K
   float vRef = 3.3;
   pinMode(vSense, INPUT);  // get ready to read voltage
   if (vRef==1.2) analogReference(INTERNAL); //1.2V ref more stable than 3.3 according to PJRC
   int navg = 32;
   for(int n = 0; n<navg; n++){
    voltage += (float) analogRead(vSense);
   }
   voltage = vDivider * vRef * voltage / 1024.0 / navg;  
   pinMode(vSense, OUTPUT);  // done reading voltage
   return voltage;
}
