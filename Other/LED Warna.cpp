#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFiClient.h>
#include <HX711.h>
#include <Wire.h>

#define PIN_D0 D0
#define PIN_D8 D8
#define StandBy 1
#define Error 2
#define MachineUsed 4
#define AuthSuccess 11
#define DataRecorded 18

int status;
int biarGaLoop; 
void setup()
{
  Serial.begin(9600);
  status = StandBy; 
  biarGaLoop = 1; 
  pinMode(PIN_D0, OUTPUT);
  pinMode(PIN_D8, OUTPUT);
}

void colourlamp(int colourCode)
{
 for(int j=0;j<colourCode;j++)
 {
   digitalWrite(PIN_D0, HIGH);
   digitalWrite(PIN_D0, LOW);
 }
  delay(500);
  digitalWrite(PIN_D8, HIGH);
  digitalWrite(PIN_D8, LOW);
}

void indicatorlamp(int lampCode)
{
    status = lampCode;
  if(lampCode<8)
  {
    colourlamp(lampCode);
  }
  else
  {
    int times = lampCode/7;
    lampCode = lampCode%7;
    for(int i=0;i<=times;i++)
    {
      colourlamp(lampCode);
      delay(1000);
    }
  }
}


void loop()
{
  if(biarGaLoop>0){
  
  indicatorlamp(StandBy);
  
  status = 20;
  
  biarGaLoop--;
  }
  indicatorlamp(status);
}

