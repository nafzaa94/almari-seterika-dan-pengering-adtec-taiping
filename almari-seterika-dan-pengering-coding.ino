#define BLYNK_TEMPLATE_ID "TMPLDY-z2stz"
#define BLYNK_DEVICE_NAME "almari pengering"
#define BLYNK_AUTH_TOKEN "08ZiUw9yK5JrdduemTGICdRg3DTTHV9g"

#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

#include "DHT.h"

#define DHTPIN 9

#define DHTTYPE DHT22

#define BLYNK_PRINT Serial

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "HP";
char pass[] = "nafza9494";

// Hardware Serial on Mega, Leonardo, Micro...
#define EspSerial Serial1

// or Software Serial on Uno, Nano...
// #include <SoftwareSerial.h>
// SoftwareSerial EspSerial(2, 3); // RX, TX

// Your ESP8266 baud rate:
#define ESP8266_BAUD 38400

ESP8266 wifi(&EspSerial);

DHT dht(DHTPIN, DHTTYPE);

int poweronoff = 0;
int startsystem = 0;
int valuefunctions;

int buttonmanual = 8;
int valuebuttonmanual = 0;

int relay1 = 2; // utk ptc heater 
int relay2 = 3; // kipas 
int relay3 = 4; // led pilot red
int relay4 = 5; // led pilot green

int relay5 = 6; // power on off
int relay6 = 7; // power on off

String Status;

int hours = 0;
int mins = 0;

int setmin = 5; 

int hoursetclose = 0;
int minsetclose = 0;

int state = 0;
int statemanual = 0;
int statelcd = 0;

int timerdisplay = 0;

BlynkTimer timer;

BLYNK_WRITE(V2)
{
  poweronoff = param.asInt();

  Serial.println(poweronoff);

  if (poweronoff == 1){
    digitalWrite(relay5, LOW);
    digitalWrite(relay6, LOW);
    digitalWrite(relay3, HIGH); // lampu merah hidup
    digitalWrite(relay4, LOW);
    }

  else {
    digitalWrite(relay5, HIGH);
    digitalWrite(relay6, HIGH);
    digitalWrite(relay3, LOW);
    digitalWrite(relay4, LOW);
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, HIGH);
    Blynk.virtualWrite(V3, 0);
  }
  
}

BLYNK_WRITE(V3)
{ 
  valuefunctions = param.asInt();

  if (valuefunctions == 1){
    startsystem = 1;
    }
  else {
    startsystem = 0;
    }
  
}

void myTimerEvent()
{
  valuebuttonmanual = digitalRead(buttonmanual);

  if (valuebuttonmanual == LOW && statemanual == 0){
    startsystem = 1;
    statemanual = 1;
    statelcd = 0;
    }

  else if (valuebuttonmanual == LOW && statemanual == 1){
    startsystem = 0;
    statemanual = 0;
    statelcd = 0;
    }
  
  if (poweronoff == 1){
    switch (startsystem) {
      case 0:
        statelcd = 0;
        digitalWrite(relay1, HIGH);
        digitalWrite(relay2, HIGH);
        digitalWrite(relay3, LOW); 
        digitalWrite(relay4, HIGH); // lampu hijau hidup
        if (statelcd == 0){
          lcd.setCursor(13, 0);
          lcd.print("       ");
          lcd.setCursor(13, 0);
          lcd.print("Not Run");
          statelcd = 1;
        }
        break;
      case 1:
        statelcd = 0;
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, LOW);
        digitalWrite(relay3, HIGH); // lampu merah hidup
        digitalWrite(relay4, LOW);
        startsystem  = 2;
        Status = "Running";
        if (statelcd == 0){
          lcd.setCursor(13, 0);
          lcd.print("       ");
          lcd.setCursor(13, 0);
          lcd.print("Running");
          statelcd = 1;
        }
        break;
      case 2:
        tmElements_t tm;
        RTC.read(tm);
      
        hours = tm.Hour;
        mins = tm.Minute;
      
        if (state == 0){
          minsetclose = mins + setmin;
          hoursetclose = hours;
        
          if (minsetclose >= 60 ){
            minsetclose = minsetclose - 60;
            hoursetclose = hoursetclose + 1;
            }
          state = 1;
          }

        timerdisplay = ((hoursetclose * 60) + minsetclose) - ((hours * 60)+ mins);

        lcd.setCursor(13, 1);
        lcd.print("  ");
        lcd.setCursor(13, 1);
        lcd.print(timerdisplay);
        lcd.setCursor(16, 1);
        lcd.print("Min");

        if (timerdisplay < 10){
          lcd.setCursor(13, 1);
          lcd.print(" ");
          lcd.setCursor(13, 1);
          lcd.print(timerdisplay);
          lcd.setCursor(15, 1);
          lcd.print("    ");
          lcd.setCursor(15, 1);
          lcd.print("Min");
          }
      
      
        if (hours == hoursetclose && mins == minsetclose){
          digitalWrite(relay1, HIGH);
          digitalWrite(relay2, HIGH);
          digitalWrite(relay3, LOW);
          digitalWrite(relay4, HIGH); // lampu hijau hidup
          startsystem = 0;
          statemanual = 0;
          state = 0;
          Status = "done";
          Blynk.virtualWrite(V3, 0);
          lcd.setCursor(13, 0);
          lcd.print("       ");
          lcd.setCursor(13, 0);
          lcd.print("Done");
          delay (2000);
          }
        break;
      
    }
  }

  float t = dht.readTemperature();
  float h = dht.readHumidity();

  Blynk.virtualWrite(V1, Status);
  Blynk.virtualWrite(V4, h);
  Blynk.virtualWrite(V5, t);
  Blynk.virtualWrite(V6, timerdisplay);
}

void setup()
{
  // Debug console
  Serial.begin(9600);
  lcd.begin(); // kalu error tukar begin jd init()
  lcd.backlight();
  pinMode(buttonmanual, INPUT_PULLUP);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  pinMode(relay5, OUTPUT);
  pinMode(relay6, OUTPUT);
  dht.begin();

  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, LOW);
  digitalWrite(relay4, LOW);
  digitalWrite(relay5, HIGH);
  digitalWrite(relay6, HIGH);

  lcd.setCursor(0, 0);
  lcd.print("Status     = ");
  lcd.setCursor(0, 1);
  lcd.print("Timer      = ");
  lcd.setCursor(0, 2);
  lcd.print("Temperature= ");
  lcd.setCursor(0, 3);
  lcd.print("Humidity   = ");
  
  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  Blynk.begin(auth, wifi, ssid, pass, "blynk.cloud", 80);

  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run();
}
