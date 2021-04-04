//Created by Giuseppe Tamanini 02/04/2021
//Licenza CC/SA
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"
#include <WS2812.h>

RTC_DS1307 rtc;
#define outputPin 8  // Digital output pin command WS2812
#define LEDCount 60   // Number of LEDs WS2812
WS2812 LED(LEDCount);
cRGB value;

byte holdminute=99;
byte holdsecond=99;
int sensorPin = A0;   // input pin per photoresistor
int sensorValue = 0;  // variable to store the value coming from the sensor
int pot1Pin = A2;
int pot1Value = 0;
int pot2Pin = A1; 
int pot2Value = 0;
float lumMin = 0;
float lumMax = 1;
float lumDev = 0;
float lumValue = 1; // luminosità dei led
int hh = 0;
int hho = 0;
int oldhh = - 99;
int mm = 0;;
int ss = 0;
boolean mh = true;
int gg = 0;
int mmm = 0;
int gs = 0;
int yy = 0;
int q = 0;
int ledore = 6;
boolean valbutton;
boolean oldvalbutton;
boolean modifica = false;
int buttonPin = 3;
boolean cambiato;
unsigned long myTime;
unsigned long myTime1;
unsigned long previousMillis;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  pinMode(ledore, OUTPUT);
  LED.setOutput(outputPin);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    rtc.adjust(DateTime(2020, 1, 1, 0, 0, 0));
  }
  previousMillis = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  DateTime now = rtc.now();
  ss = now.second(); 
  if (holdsecond != ss) {
    sensorValue = analogRead(sensorPin);
    Serial.print("sensorValue=");
    Serial.println(sensorValue);
    pot1Value =  analogRead(pot1Pin);
    pot2Value =  analogRead(pot2Pin);
    lumMin = pot1Value / 1024.00;
    lumMax = pot2Value / 1024.00;
    Serial.print("lunMin=");
    Serial.println(lumMin);
    Serial.print("lunMax=");
    Serial.println(lumMax);
    lumDev = (lumMax + 1 - lumMin) / 2;
    lumValue = lumMin * 0.5 + lumDev * sensorValue / 1024.00;
    Serial.print("lumDev=");
    Serial.println(lumDev);
    Serial.print("lumValue=");
    Serial.println(lumValue);
    mm = now.minute();
    hh = now.hour();
    hho = hh;
    gg = now.day();
    mmm = now.month();
    yy = now.year();
    gs = now.dayOfTheWeek();
    if (gs == 0) gs = 7;
    if (hh > 11) {
      hho = hh - 12;
    }
    hho = hho * 5 + mm / 12 + 0.5;
    holdsecond = ss;
    analogWrite(ledore, 255 * lumValue);
    AccendiOre();
  }
  valbutton = digitalRead(buttonPin);
  if (valbutton && !oldvalbutton) {
    myTime = millis();
    oldvalbutton = true; 
  } else if (valbutton == false && oldvalbutton) {
    oldvalbutton = false;  // pone il vecchio stato del pulsante a falso
    cambiato = false; // lo stato dell'azione del pulsante1 è falsa 
  }
  if (valbutton && oldvalbutton && cambiato == false && millis() - myTime > 1000) {
    myTime = millis();
    while (valbutton) { // ripete finché il pulsante1 è ancora premuto attendendone il rilascio
      valbutton = digitalRead(buttonPin);
    }
    oldvalbutton = false;
    cambiato = false;
    modifica = true;
    myTime1 = millis();
    modificaora(); // esegue la funzione modificadata
    Serial.println("Modificaora");
  } 
}


void AccendiOre () {
  for (int i = 0; i < 60; i++) {
    value.b = 1; value.g = 1; value.r = 0;
    LED.set_crgb_at(i, value);
  }
  int mp = mm - 1;
  int ms = mm + 1;
  if (mp < 0) mp = 59;
  if (ms == 60) ms = 0;
  value.b = 0; value.g = 16; value.r = 2;
  LED.set_crgb_at(mp, value);
  value.b = 0; value.g = 127; value.r = 16;
  LED.set_crgb_at(mm, value);
  value.b = 0; value.g = 16; value.r = 2;
  LED.set_crgb_at(ms, value);
  int hp = hho - 1;
  int hs = hho + 1;
  if (hp < 0) hp = 59;
  if (hs == 60) hs = 0;
  if (hp != mm) {
    value.b = 0; value.g = 0; value.r = 2;
    LED.set_crgb_at(hp, value);    
  }
  value.b = 0; value.g = 0; value.r = 127;
  LED.set_crgb_at(hho, value);
  if (hs != mm) {
    value.b = 0; value.g = 0; value.r = 2;
    LED.set_crgb_at(hs, value);
  }
  if (mm == hho && mh == true) {
    value.b = 0; value.g = 16; value.r = 2;
    LED.set_crgb_at(mp, value);
    value.b = 0; value.g = 127; value.r = 16;
    LED.set_crgb_at(mm, value);
    value.b = 0; value.g = 16; value.r = 2;
    LED.set_crgb_at(ms, value);
    mh = false;
  }
  else if (mm == hho && mh == false) {
    value.b = 0; value.g = 0; value.r =2;
    LED.set_crgb_at(hp, value);
    value.b = 0; value.g = 0; value.r =127;
    LED.set_crgb_at(hho, value);
    value.b = 0; value.g = 0; value.r =2;
    LED.set_crgb_at(hs, value);
    mh = true;
  }
  value.b = 255; value.g = 0; value.r = 0;
  LED.set_crgb_at(ss, value);
  LED.sync(); // Sends the value to the LED
}

void modificaora() {
  while (modifica) {
  for (int i = 0; i < 60; i++) {
    value.b = 1; value.g = 1; value.r = 0;
    LED.set_crgb_at(i, value);
  }
  int mp = mm - 1;
  int ms = mm + 1;
  if (mp < 0) mp = 59;
  if (ms == 60) ms = 0;
  value.b = 0; value.g = 16; value.r = 2;
  LED.set_crgb_at(mp, value);
  value.b = 0; value.g = 127; value.r = 16;
  LED.set_crgb_at(mm, value);
  value.b = 0; value.g = 16; value.r = 2;
  LED.set_crgb_at(ms, value);
  int hp = hho - 1;
  int hs = hho + 1;
  if (hp < 0) hp = 59;
  if (hs == 60) hs = 0;
  if (hp != mm) {
    value.b = 0; value.g = 0; value.r = 2;
    LED.set_crgb_at(hp, value);    
  }
  value.b = 0; value.g = 0; value.r = 127;
  LED.set_crgb_at(hho, value);
  if (hs != mm) {
    value.b = 0; value.g = 0; value.r = 2;
    LED.set_crgb_at(hs, value);
  }
  if (mm == hho && mh == true) {
    value.b = 0; value.g = 16; value.r = 2;
    LED.set_crgb_at(mp, value);
    value.b = 0; value.g = 127; value.r = 16;
    LED.set_crgb_at(mm, value);
    value.b = 0; value.g = 16; value.r = 2;
    LED.set_crgb_at(ms, value);
    mh = false;
  }
  else if (mm == hho && mh == false) {
    value.b = 0; value.g = 0; value.r =2;
    LED.set_crgb_at(hp, value);
    value.b = 0; value.g = 0; value.r =127;
    LED.set_crgb_at(hho, value);
    value.b = 0; value.g = 0; value.r =2;
    LED.set_crgb_at(hs, value);
    mh = true;
  }
  LED.sync(); // Sends the value to the LED
  valbutton = digitalRead(buttonPin);
  if (valbutton && !oldvalbutton) {
    myTime = millis();
    oldvalbutton = true;
    cambiato = true;
  } else if (valbutton == false && oldvalbutton) {
    oldvalbutton = false;  // pone il vecchio stato del pulsante a falso
    cambiato = false;
    myTime1 = millis();
  }
  if (valbutton && oldvalbutton && millis() - myTime > 50) {
    Serial.println(valbutton);
    myTime = millis();
    oldvalbutton = true;
    mm = mm + 1;
    if (mm == 60) {
      mm = 0;
      hh = hh + 1;
      if (hh == 24) hh = 0;
    }
    hho = hh;
    if (hh > 11) {
      hho = hh - 12;
    }
    hho = hho * 5 + mm / 12 + 0.5;
  }
  if (valbutton == false && oldvalbutton == false && cambiato == false && millis() - myTime1 > 3000) {
    rtc.adjust(DateTime(yy, mmm, gg, hh, mm, ss));
    modifica = false;
  }
  }
}
