#define BLYNK_TEMPLATE_ID "TMPLgihD0BWQ"
#define BLYNK_DEVICE_NAME "TEST ULTRASONIC"
#define BLYNK_AUTH_TOKEN "qDgRws8jt0MGBRkSBWNJunAaje6kcyQq"

#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

#include "DHT.h"

#define DHTPIN 9

#define DHTTYPE DHT22

#define BLYNK_PRINT Serial

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "YourNetworkName";
char pass[] = "YourPassword";

// Hardware Serial on Mega, Leonardo, Micro...
#define EspSerial Serial1

// or Software Serial on Uno, Nano...
// #include <SoftwareSerial.h>
// SoftwareSerial EspSerial(2, 3); // RX, TX

// Your ESP8266 baud rate:
#define ESP8266_BAUD 9600

ESP8266 wifi(&EspSerial);

DHT dht(DHTPIN, DHTTYPE);


int poweronoff = 0;
int functions = 0;
const char* valuefunctions;

int relay1 = 2;
int relay2 = 3;
int relay3 = 4;
int relay4 = 5;
int relay5 = 6;
int relay6 = 7;

String Status;

int hours = 0;
int mins = 0;

int setmin = 45; 

int hoursetclose = 0;
int minsetclose = 0;

BlynkTimer timer;

BLYNK_WRITE(V2)
{
  poweronoff = param.asInt();

  if (poweronoff = 1){
    digitalWrite(relay5, LOW);
    }

  else {
    digitalWrite(relay5, HIGH);
  }
  
}

BLYNK_WRITE(V3)
{ 
  valuefunctions = param.asStr();

  if (valuefunctions == "1"){
    functions = 1;
    }
 else if (valuefunctions == "2"){
    functions = 2;
  }
 else {
    functions = 3;
  }
  
}

void myTimerEvent()
{
  
  if (poweronoff == 1){
    switch (functions) {
      case 1:
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, LOW);
        functions  = 4;
        Status = "Running";
        break;
      case 2:
        digitalWrite(relay3, LOW);
        digitalWrite(relay4, LOW);
        functions  = 4;
        Status = "Running";
        break;
      case 3:
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, LOW);
        digitalWrite(relay3, LOW);
        digitalWrite(relay4, LOW);
        functions  = 4;
        Status = "Running";
        break;
      case 4:
        tmElements_t tm;
        RTC.read(tm);
      
        hours = tm.Hour;
        mins = tm.Minute;
      
        minsetclose = mins + setmin;
        hoursetclose = hours;
      
        if (minsetclose >= 60 ){
          minsetclose = minsetclose - 60;
          hoursetclose = hoursetclose + 1;
          }
      
      
        if (hours == hoursetclose && mins == minsetclose){
          digitalWrite(relay1, HIGH);
          digitalWrite(relay2, HIGH);
          digitalWrite(relay3, HIGH);
          digitalWrite(relay4, HIGH);
          functions = 0;
          Status = "done";
          }
        break;
      
    }
  }

  float t = dht.readTemperature();
  float h = dht.readHumidity();

  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, Status);
  Blynk.virtualWrite(V4, h);
}

void setup()
{
  // Debug console
  Serial.begin(9600);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  pinMode(relay5, OUTPUT);
  pinMode(relay6, OUTPUT);
  dht.begin();

  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);
  digitalWrite(relay4, HIGH);
  digitalWrite(relay5, HIGH);
  digitalWrite(relay6, HIGH);
  
  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  Blynk.begin(auth, wifi, ssid, pass);

  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run();
}
