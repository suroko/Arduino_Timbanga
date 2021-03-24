// 1.1 mengganti authentication function menjadi boolean dan returnnya
#define ARDUINOJSON_ENABLE_COMMENTS 1

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFiClient.h>
#include <HX711.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
int lastCln;
int lastRow;

const int buttonPin = 0;     // the number of the pushbutton pin

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
void display(String massage, int cln, int row, bool isClear, bool isCont, bool isCenter) {

  if (!isClear && isCont && cln != lastCln && !isCenter) {
    lcd.setCursor(cln, row);
    lcd.print(massage);

  } else if (!isClear && isCont && cln != lastCln && isCenter) {
    int length = massage.length() - 1;
    int pad = (length >= 16) ? 0 : (16 - length) / 2;
    lcd.setCursor(pad, row);
    lcd.print(massage);

  } else if (!isClear && isCont && row != lastRow && !isCenter) {
    lcd.setCursor(cln, row);
    lcd.print(massage);

  } else if (!isClear && isCont && row != lastRow && isCenter) {
    int length = massage.length() - 1;
    int pad = (length >= 16) ? 0 : (16 - length) / 2;
    lcd.setCursor(pad, row);
    lcd.print(massage);

  }else if (!isClear && isCont) {
    lcd.print(massage);

  } else if (isClear) {
    lcd.clear();
    lcd.setCursor(cln, row);
    lcd.print(massage);

  } else {
    lcd.clear(); 
    display("Display", 1, 0, true, false, false);
    display("Error dd001", 2, 0, false, true, false);
    delay(1500);
    return;
  }
  lastCln = cln;
  lastRow = row;

}

void setup() {
  Serial.begin(115200);
  pinMode(A0, INPUT_PULLUP);
  lcd.init();
  lcd.backlight();
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
}

void loop() {
  // read the state of the pushbutton value:
  buttonState = analogRead(0);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  Serial.println(buttonState);
  if (buttonState > 800) {
    // turn LED on:
    display("ButtonNyala", 1, 0, true, false, false);
  } else {
    // turn LED off:
    display("Button Mati", 1, 0, true, false, false);
  }
  delay(200);
}