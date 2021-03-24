#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFiClient.h>
#include <HX711.h>
#include <Wire.h>

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

#define PIN_D0 D7
#define PIN_D8 D8
#define StandBy 1
#define Error 2
#define MachineUsed 4
#define AuthSuccess 11
#define DataRecorded 18

int status;
// RFID
MFRC522 rfid(SDA_PIN, RST_PIN);



void setup() {
    Serial.begin(9600);
    delay(100);
    pinMode(LED_PIN_D0, OUTPUT);
    pinMode(LED_PIN_D8, OUTPUT);
    
    // SPI.begin();
    // rfid.PCD_Init();
    //indicator
}

void colorlamp(int colourCode) {
  for(int j=0;j<colourCode;j++) {
    digitalWrite(LED_PIN_D0, HIGH);
    digitalWrite(LED_PIN_D0, LOW);
    }
  delay(100);
  digitalWrite(LED_PIN_D8, HIGH);
  digitalWrite(LED_PIN_D8, LOW);
}

void indicatorlamp(int lampCode) {
  status = lampCode;
  if(lampCode<8) {
    colorlamp(lampCode);
  }
  else {
    int times = lampCode/7;
    lampCode = lampCode%7;
    for(int i=0;i<=times;i++) {
      colorlamp(lampCode);
      delay(1000);  
    }
  }
}

void connectWifi(){
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Waiting to be connected...");
    }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Netmask: ");
  Serial.println(WiFi.subnetMask());
  Serial.println("Gateway: ");
  Serial.println(WiFi.gatewayIP());
}

void loop() {
  indicatorlamp(StandBy);
  delay(1000);
  if(WiFi.status() == WL_CONNECTED){
  Serial.println("wificonnect!!!!");
  }
  else if(WiFi.status() == WL_DISCONNECTED){
    Serial.println("diskonek");
    delay(2000);
    connectWifi();
  }
  else{
  Serial.println("failed!!!!");
  connectWifi();
  }
    
    // if ( ! rfid.PICC_IsNewCardPresent())
    // return;

    // if ( ! rfid.PICC_ReadCardSerial())
    // return;
}