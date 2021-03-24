#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFiClient.h>
#include <HX711.h>

// PIN LED
#define PIN_D0 D0
#define PIN_D8 D8

// PIN RFID 
#define RST_PIN D1
#define SDA_PIN D2

// PIN HX711 circuit wiring
#define LOADCELL_DOUT_PIN D4
#define LOADCELL_SCK_PIN D3

// Colour Code
#define Prepare 1
#define WifiConnected 2
#define MachineUsed 4//4
#define ConnectWifi 2//37
#define AuthFailed 1//36

int status;

//1 MERAH -> baru nyala
//2 merah mati, kuning kedip2 -> konek wifi
//3 Kuning -> Connected + ready
//4 Hijau -> RFID detected, ready for transaction

const char* ssid = "Kudu bisa!~"; //WiFi SSID
const char* password = "pasti bisa"; //WiFi Password
const char* host = "192.168.43.250"; //Host 
String condition = "";

// RFID
MFRC522 mfrc522(SDA_PIN, RST_PIN);

// scale
HX711 scale;
float calibration_factor = -244.0;
float weight;


void setup() {
  
  indicatorlamp(Prepare);
  Serial.begin(9600);
  delay(100);

  //1. Connect to WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Waiting to be connected...");
  }
  indicatorlamp(ConnectWifi);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Netmask: ");
  Serial.println(WiFi.subnetMask());
  Serial.println("Gateway: ");
  Serial.println(WiFi.gatewayIP());

  indicatorlamp(WifiConnected);
  //2. Prepare Scale
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale();
  scale.tare();

  //3. Prepare RFID
  SPI.begin(); // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  Serial.println("Put your card to the reader..."); //shows in serial that it is ready to read

}

void loop() {
  
  while (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    indicatorlamp(MachineUsed);
    Serial.print("UID tag :");
    String content = "";

    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    Serial.print("Pesan : ");
    content.toUpperCase();

    if (content.substring(0) == "406A5F90") {
      Serial.println("Kartu cocok");
      Serial.println();
      delay(1000);

      if (scale.is_ready()) {
        scale.set_scale(calibration_factor);

        while(!Serial.available()){
        weight = scale.get_units();
        Serial.print("Weight : ");
        if (weight <= 0.1) {
          weight = 0.0;
        }
        Serial.println(weight *= 100);
        Serial.println("Push Button to stop weight ");
        condition = Serial.readString();
        delay(500);
        weight = 0.0;
        }
      } else {
        Serial.println(" HX711 not found.");
      }

    } else {
      Serial.println("Kartu Tidak cocok");
      delay(1000);
    }
    // HX711
    String cid = content.substring(0);
    String url = "/projectkeren/record.php?card=" + String(cid) + "&w=" + weight;
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
    Serial.print(String("GET ") + url + " HTTP/1.1\r\n" +
      "Host: " + host + "\r\n" + "Connection: close\r\n\r\n"); // wait for data to be available unsigned long

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        delay(60000);
        return;
      }
    }

    // Read all the lines of the reply from server and print them to Serial
    Serial.println("receiving from remote server");
    // not testing 'client.connected()' since we do not need to send data here
    String s;
    while (client.available()) {
      char ch = static_cast < char > (client.read());
      s.concat(ch);
      //Serial.print(ch)
    }
    Serial.print(s);

    delay(500);
    }

    // if (!mfrc522.PICC_IsNewCardPresent()){
    //   indicatorlamp(AuthFailed);
    // }
    indicatorlamp(WifiConnected);

}