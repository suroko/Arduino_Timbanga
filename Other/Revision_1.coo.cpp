#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFiClient.h>
#include <HX711.h>

// PIN LED
#define LED_PIN_D0 D0
#define LED_PIN_D8 D8

// PIN RFID 
#define RST_PIN D1
#define SDA_PIN D2

// PIN HX711 circuit wiring
#define LOADCELL_DOUT_PIN D4
#define LOADCELL_SCK_PIN D3

const char* ssid = "Kudu bisa!~"; //WiFi SSID
const char* password = "pasti bisa"; //WiFi Password
const char* host = "192.168.43.250"; //Host 
String condition = "";

// RFID
MFRC522 rfid(SDA_PIN, RST_PIN);


void setup() {
    pinMode(LED_PIN_D0, OUTPUT);
    pinMode(LED_PIN_D8, OUTPUT);
    
    SPI.begin();
    rfid.PCD_Init();
    //indicator
}

void ConnectWifi(){
    WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Waiting to be connected...");
    //indicator
  }
}

void loop() {
    
    // if ( ! rfid.PICC_IsNewCardPresent())
    // return;

    // if ( ! rfid.PICC_ReadCardSerial())
    // return;
}