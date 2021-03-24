/* Import Package */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h> 
#include <WiFiClient.h> 
#include <HX711.h> 
#include <Wire.h> 
#include <LiquidCrystal_I2C.h> 
#include <ArduinoJson.h>

// constants won't change. They're used here to set pin numbers:
const int BUTTON_PIN = 7; // the number of the pushbutton pin
const int SHORT_PRESS_TIME = 500; // 1000 milliseconds
const int LONG_PRESS_TIME  = 900; // 1000 milliseconds

// Variables will change:
int lastState = 0;  // the previous state from the input pin
int currentState;     // the current reading from the input pin
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;
bool isPressing = false;
bool isLongDetected = false;

void setup() {
  Serial.begin(115200);
  pinMode(A0, INPUT_PULLUP);
}

void loop() {
  // read the state of the switch/button:
  currentState = ((analogRead(A0)>500) ? HIGH : LOW);
    // Serial.println(currentState);
    // Serial.println("currentState :");
    // Serial.println(analogRead(A0));
  if(lastState == LOW && currentState == HIGH) {        // button is pressed
    pressedTime = millis();
    //Serial.print("\n pressedTime ");
    //Serial.print(pressedTime);
    isPressing = true;
    isLongDetected = false;
  } else if(lastState == HIGH && currentState == LOW) { // button is released
    isPressing = false;
    releasedTime = millis();

    long pressDuration = releasedTime - pressedTime;
    //Serial.print("\n pressedTime 2 ");
   // Serial.print(pressDuration);
    if( pressDuration < SHORT_PRESS_TIME )
      Serial.println("A short press is detected");
  }

  if(isPressing == true && isLongDetected == false) {
    long pressDuration = millis() - pressedTime;

    //Serial.print("\n pressedTime 3 ");
    //Serial.print(pressDuration);
    if( pressDuration > LONG_PRESS_TIME ) {
      Serial.println("A long press is detected");
      isLongDetected = true;
    }
  }
  // save the the last state
  lastState = currentState;
}