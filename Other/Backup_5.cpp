#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFiClient.h>
#include <HX711.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

// PIN RFID 
#define RST_PIN D4
#define SDA_PIN D3

// PIN HX711 circuit wiring
#define LOADCELL_DOUT_PIN D8
#define LOADCELL_SCK_PIN D0

const char* ssid = "laptop"; //WiFi SSID
const char* password = "laptop123"; //WiFi Password
const char* host = "192.168.137.1"; //Host 
String condition = "";
int lastCln;
int lastRow;

bool standbyMode = false;
// RFID
MFRC522 rfid(SDA_PIN, RST_PIN);

LiquidCrystal_I2C lcd(0x27,16,2);

void display(String massage, int cln, int row, bool isClear, bool isCont){

  if(!isClear && isCont && cln != lastCln ){
    lcd.setCursor(cln,row);
    lcd.print(massage);

  }else if(!isClear && isCont && row != lastRow ){
    lcd.setCursor(cln,row);
    lcd.print(massage);

  }else if(!isClear && isCont){
    lcd.print(massage);

  }else if(isClear){
    lcd.clear();
    lcd.setCursor(cln,row);
    lcd.print(massage);

  }else{
    lcd.setCursor(cln,row);
    lcd.print("No option");
  }
  lastCln = cln;
  lastRow = row;

  
}

void ConnectWifi(){

  Serial.print("\nConnecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Waiting to be connected...");
    display("Wifi ",0,0,true,false);
    display(ssid,0,0,false,true);
    display("Connecting",1,1,false,true);
    for (int i = 0; i < 3; i++)
    {
      display(".",1,1,false,true);
      delay(1000);
    }
  }

  Serial.println("\nWiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Netmask: ");
  Serial.println(WiFi.subnetMask());
  Serial.println("Gateway: ");
  Serial.println(WiFi.gatewayIP());
  standbyMode = false;
}

void setup() {
  Serial.begin(115200);
  delay(100);
  
  lcd.init();          
  lcd.backlight();
  display("Cek lcd",3,0,true,false);
  for (int i = 0; i < 3; i++)
  {
    display(".",3,0,false,true);
    delay(1000);
  }
  display("Berhasil",4,1,false,true);
  delay(1500);
  display("Cek RFID",3,0,true,false);
  SPI.begin();
  rfid.PCD_Init();
  for (int i = 0; i < 3; i++)
  {
    display(".",3,0,false,true);
    delay(1000);
  }
  display("Berhasil",4,1,false,true);
  delay(1500);

  ConnectWifi();
}

void authentication(String content){
  String cid = content.substring(0);
  String url = "/projectkeren/Authentication.php?card=" + String(cid);
  Serial.print("Requesting URL: ");
  Serial.println(url);
  WiFiClient client;
  // Use WiFiClient class to create TCP connections
  if (!client.connect(host, 80)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
      "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      delay(60000);
      return;
    }
  }

  Serial.println("receiving from remote server");

  String s;
    while (client.available()) {
      char ch = static_cast < char > (client.read());
      s.concat(ch); 
      
    }

    Serial.print(s);
}


void sendData(String content){
  Serial.println();
  String cid = content.substring(0);
  String url = "/projectkeren/record.php?card=" + String(cid) + "&w=" + 0;
  Serial.print("Requesting URL: ");
  Serial.println(url);
  WiFiClient client;
  // Use WiFiClient class to create TCP connections
  if (!client.connect(host, 80)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
      "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      delay(60000);
      return;
    }
  }

  Serial.println("receiving from remote server");

  const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
  DynamicJsonDocument doc(capacity);
  String s;
  Serial.print("\n batas \n");
    while (client.available()) {
      char ch = static_cast < char > (client.read());
      Serial.print(client.read());
      Serial.print("\n batas \n");
      Serial.print(ch);
      Serial.print("\n batas \n");
      Serial.print(client.find("name"));
      Serial.print("\n batas \n");
      s.concat(ch); 

       if(client.find("name")){
      //   display("Hello ",1,0,true,false);
      //   display(doc["name"].as<char*>(),1,0,false,true);
      //   delay(1500);
      // }
    }
    Serial.print(s);
}

void loop() {

  if(WiFi.status() != WL_CONNECTED){
    ConnectWifi();
  }

  Serial.println("\nPut your card to the reader...");
  delay(500);
  if (!standbyMode)
  {
    display("Put Your Card",1,0,true,false);
    standbyMode = true;
  }

  if ( ! rfid.PICC_IsNewCardPresent())
  return;

  if ( ! rfid.PICC_ReadCardSerial())
  return;
  
  
  display("Reading",1,1,false,true);
  for (int i = 0; i < 3; i++)
  {
    display(".",1,1,false,true);
    delay(1000);
  }
  Serial.print("UID tag :");
  String content = "";

  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
    Serial.print(rfid.uid.uidByte[i], HEX);
    
    content.concat(String(rfid.uid.uidByte[i] < 0x10 ? "0" : ""));
    content.concat(String(rfid.uid.uidByte[i], HEX));
  }
  display("Reading",4,0,true,false);
  display("Successful",3,1,false,true);
  standbyMode = false;
  delay(1500);
  content.toUpperCase();
  authentication(content);
}

