
 ESP32 + Blynk IoT (8-Relay + Switches + DHT + AUTO MODE) 

#define BLYNK_TEMPLATE_ID   TMPL3wWnm2Czo
#define BLYNK_TEMPLATE_NAME Smart Home
#define BLYNK_AUTH_TOKEN    4Ul8FkBYB4We5cHfPnf3s4bmsCwwXec6

#define BLYNK_PRINT Serial

#include WiFi.h
#include BlynkSimpleEsp32.h
#include DHT.h

 ---------- Gas Sensor ----------
#define GAS_SENSOR_PIN 34       AO pin
#define BUZZER_PIN 17


 ---------- WiFi ----------
const char WIFI_SSID = 12345678;
const char WIFI_PASS = 12345678;

 ---------- Relay ----------
const bool RELAY_ACTIVE_LOW = true;
const uint8_t RELAY_PINS[8] = {5,18,19,21,22,23,25,26};

 ---------- Physical Switch Pins ----------
const uint8_t SWITCH_PINS[8] = {32,33,27,14,12,13,15,4};

int gasThreshold = 500;    Adjust after testing

 ---------- DHT ----------
#define DHTPIN 16
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

 ---------- States ----------
uint8_t relayState[8]={0};
uint8_t lastSwitchState[8]={1};

bool autoMode=false;
float targetTemp=28;
float targetHumidity=60;

BlynkTimer timer;

 ---------- Relay ----------
void setRelay(uint8_t ch,bool on){
  relayState[ch]=on;
  digitalWrite(RELAY_PINS[ch],RELAY_ACTIVE_LOW!onon);
  Blynk.virtualWrite(ch,on);
}

 ---------- Physical Switch ----------
void readSwitches(){
  for(int i=0;i8;i++){
    uint8_t sw=digitalRead(SWITCH_PINS[i]);
    if(sw!=lastSwitchState[i]){
      delay(30);
      if(digitalRead(SWITCH_PINS[i])==sw){
        lastSwitchState[i]=sw;
        if(sw==LOW){
          if(autoMode && (i==3  i==4  i==5)) continue;
          setRelay(i,!relayState[i]);
        }
      }
    }
  }
}

 ---------- Relay Buttons ----------
#define MAKE_BLYNK_WRITE(vpin,ch) 
BLYNK_WRITE(vpin){ 
 if(autoMode && (ch==3  ch==4  ch==5)) return; 
 setRelay(ch,param.asInt()); 
}

MAKE_BLYNK_WRITE(V0,0)
MAKE_BLYNK_WRITE(V1,1)
MAKE_BLYNK_WRITE(V2,2)
MAKE_BLYNK_WRITE(V3,3)
MAKE_BLYNK_WRITE(V4,4)
MAKE_BLYNK_WRITE(V5,5)
MAKE_BLYNK_WRITE(V6,6)
MAKE_BLYNK_WRITE(V7,7)

 ---------- Auto Settings ----------
BLYNK_WRITE(V20){
  autoMode=param.asInt();
  Serial.println(autoModeAUTO MODE ONAUTO MODE OFF);
}

BLYNK_WRITE(V21){
  targetTemp=param.asFloat();
}

BLYNK_WRITE(V22){
  targetHumidity=param.asFloat();
}

BLYNK_WRITE(V32)
{
  gasThreshold = param.asInt();

  Serial.print(Gas Threshold = );
  Serial.println(gasThreshold);
}

BLYNK_CONNECTED(){
  Blynk.syncVirtual(V0,V1,V2,V3,V4,V5,V6,V7,
                  V20,V21,V22,V32);
}

 ---------- DHT ----------
void sendDHT(){

  float h=dht.readHumidity();
  float t=dht.readTemperature();

  Serial.print(Temperature = );
  Serial.println(t);

  Serial.print(Humidity = );
  Serial.println(h);

  if(isnan(h)isnan(t)) return;

  Serial.printf(Temp %.1f C  Humidity %.1f %%n,t,h);

  Blynk.virtualWrite(V10,t);
  Blynk.virtualWrite(V11,h);

  if(autoMode){

     FAN CH4
    if(ttargetTemp+1) setRelay(3,true);
    else if(ttargetTemp-1) setRelay(3,false);

     HEATER CH5
    if(ttargetTemp-1) setRelay(4,true);
    else if(ttargetTemp+1) setRelay(4,false);

     HUMIDIFIER CH6
    if(htargetHumidity-3) setRelay(5,true);
    else if(htargetHumidity+3) setRelay(5,false);
  }
}

void readGasSensor()
{
  int gasValue = analogRead(GAS_SENSOR_PIN);

  Serial.print(Gas Value );
  Serial.println(gasValue);

  Blynk.virtualWrite(V31, gasValue);

  if(gasValue  gasThreshold)
  {
    digitalWrite(BUZZER_PIN, HIGH);
    Blynk.virtualWrite(V30, 1);
  }
  else
  {
    digitalWrite(BUZZER_PIN, LOW);
    Blynk.virtualWrite(V30, 0);
  }
}


void setup(){

  Serial.begin(115200);

   Relay & Switch
  for(int i=0;i8;i++){
    pinMode(RELAY_PINS[i], OUTPUT);
    pinMode(SWITCH_PINS[i], INPUT_PULLUP);
    digitalWrite(RELAY_PINS[i], RELAY_ACTIVE_LOW  HIGH  LOW);
  }

   DHT Sensor
  dht.begin();

   Gas Sensor & Buzzer
  pinMode(GAS_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

   WiFi & Blynk
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS);

   Timers
  timer.setInterval(200L, readSwitches);
  timer.setInterval(3000L, sendDHT);
  timer.setInterval(1000L, readGasSensor);
}

void loop(){
  Blynk.run();
  timer.run();
}
