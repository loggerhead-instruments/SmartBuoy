#define LED 13
#define BUOY 1
#define HWSERIAL Serial2 // GPS

long gpsTimeout;
long gpsTimeOutThreshold = 60; // wait this long trying to get GPS before quitting. This corresponds to number of 'GPRMC' lines. So approximately seconds.
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
  

}

int counter;
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
    if(gpsTimeout >= gpsTimeOutThreshold) break;
  }
  HWSERIAL.print('*');
  HWSERIAL.print(BUOY);
  HWSERIAL.print(":");
  // only send GPS if good reading; if bad GPS will only send buoy #
  if(goodGPS){  
    HWSERIAL.print(latitude);
    HWSERIAL.print(" ");
    HWSERIAL.print(longitude);
  }
  HWSERIAL.print('!');
}
