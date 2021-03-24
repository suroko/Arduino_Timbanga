#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFiClient.h>
#include <HX711.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

int a = 0;
int lastnumb;
int times = 0;
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  pinMode(A0, INPUT_PULLUP);
}

// the loop routine runs over and over again forever:
void loop() {
  a = analogRead(0);
  Serial.println(a);
  delay(250);


    a = analogRead(0);
    if(lastnumb == a){
      lastnumb = a;
      times++;  
    }else{
      lastnumb = a;
      times = 0;
    }
    
    if(times>3){
      Serial.print("berhasil");
        times = 0;
    }

}
