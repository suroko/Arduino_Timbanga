#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h> 
#include <WiFiClient.h> 
#include <HX711.h> 
#include <Wire.h> 
#include <LiquidCrystal_I2C.h> 
#include <ArduinoJson.h>

/* Pin Declaration */
#define RFID_RST_PIN D4
#define RFID_SDA_PIN D3
#define LOADCELL_DOUT_PIN D0
#define LOADCELL_SCK_PIN D8

/* Machine Information */
String address = "Jl. Abdinegara";
const char * host = "tempatsampah.online";
const String projectName = "ArduinoKelurahan";

/* Loadcell */
HX711 scale;
float weight;
float avg_result = 0;
float calibration_factor = 354840;

/* Button */
// constants won't change. They're used here to set pin numbers:
const int BUTTON_PIN = 7; // the number of the pushbutton pin
const int SHORT_PRESS_TIME = 500; // 500 milliseconds
const int LONG_PRESS_TIME  = 900; // 900 milliseconds

// Variables will change:
int lastState = 0;  // the previous state from the input pin
int currentState;     // the current reading from the input pin
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;
bool isPressing = false;
bool isLongDetected = false;

/* Wifi */
const char * ssid = "Kudu bisa!~";
const char * password = "pasti bisa"; 

/* RFID */
MFRC522 rfid(RFID_SDA_PIN, RFID_RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;

/* LCD */
LiquidCrystal_I2C lcd(0x27, 16, 2);

/* Addition */
int buttonState = 0;
int lastnumb;
int times = 0;
String condition = "";
int lastCln;
int lastRow;
String idTemp = "";
bool standbyMode = false;
bool usedMode = false;

/* New */
String id = "";
int hitung = 0;

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

  } else if (!isClear && isCont) {
    lcd.print(massage);

  } else if (isClear && isCenter) {
    lcd.clear();
    int length = massage.length() - 1;
    int pad = (length >= 16) ? 0 : (16 - length) / 2;
    lcd.setCursor(pad, row);
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

void CekIdKartu(){
/* Mesin akan menampilkan pilihan pesan "Cek ID Kartu", "Cek Saldo", "Menimbang Sampah Organik", 
* dan "Menimbang Kompos" pada baris pertama di LCD secara bergantian, dengan menggunakan tombol 
* untuk mengganti pilihannya, disertai dengan pesan "Pilih Menu" pada baris kedua
*/

/* Pengguna memilih pilihan "Cek Saldo" dengan menekan tombol selama 2 detik */
  boolean isDetect = true;
  hitung = 0;

  do{
    hitung++;
    display("Mangga Kartuna", 1, 0, true, false, false);
      display("di Tap", 4, 1, false, true, true);
    delay(250);
      display("o", 3, 1, false, true, false);
      display("o", 12, 1, false, true, false);
    delay(700);
    currentState = ((analogRead(A0)>500) ? HIGH : LOW);
    delay(100);

    if(lastState == LOW && currentState == HIGH) {        // button is pressed
      pressedTime = millis();
      isPressing = true;
      isLongDetected = false;

    } else if(lastState == HIGH && currentState == LOW) { // button is released
      isPressing = false;
    }

    if(isPressing == true && isLongDetected == false) {
      long pressDuration = millis() - pressedTime;

      if( pressDuration > LONG_PRESS_TIME ) {
        isLongDetected = true;
        return; 
      }
    }
    lastState = currentState; // save the the last state

    if(hitung == 5){
        display("Untuk Kembali", 2, 0, true, false, true);
            display("Tekan Tombol", 1, 1, false, true, true);
        delay(250);
            display("o", 0, 1, false, true, false);
            display("o", 15, 1, false, true, false);
        delay(1200);
        hitung = 0;
    }

    /* Mesin mendeteksi kartu RFID yang di tempelkan */
    if (rfid.PICC_IsNewCardPresent()){
      Serial.print("Terdeteksi");
      if (rfid.PICC_ReadCardSerial())
        isDetect = false;
    }
  }while(isDetect);

  display("o Sakedap di o", 2, 0, true, false, true);
      display("parios heula", 1, 1, false, true, true);
  delay(250);
      display("o", 0, 1, false, true, false);
      display("o", 15, 1, false, true, false);
  delay(1200);

  Serial.print("UID tag :");
  id = "";

  for (byte i = 0; i < rfid.uid.size; i++) {
      Serial.print(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
      Serial.print(rfid.uid.uidByte[i], HEX);

      id.concat(String(rfid.uid.uidByte[i] < 0x10 ? "0" : ""));
      id.concat(String(rfid.uid.uidByte[i], HEX));
  }
  standbyMode = false;
  id.toUpperCase();

  delay(1500);
  display("o ID Kartu o", 2, 0, true, false, true);
  delay(250);
      display(id, 0, 1, false, true, true);
  delay(4000);
    
}

/* ------------------------------------ */
void ConnectWifi() {

  Serial.print("\nConnecting to \n" + String(ssid));

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("\nWaiting to be connected...");
    display("Konek", 5, 0, true, false, true);
    display("Wifi ", 0, 1, true, false, false);
    display(ssid, 0, 1, false, true, false);
    for (int i = 0; i < 3; i++) {
      display(".", 5, 0, false, true, false);
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


void setup(){
    Serial.begin(115200);
    delay(100);
    pinMode(A0, INPUT_PULLUP);
    bool needRecheck = false;
    /* LCD Checking */
    do{
        lcd.blink();
        lcd.init();
        lcd.backlight();
        display("Cek lcd", 1, 0, true, false, true);
        delay(400);
        display("o", 3, 0, false, true, false);
        delay(400);
        display("o", 13, 0, false, true, false);
        delay(900);
            display("Sukses", 6, 1, false, true, false);
        delay(1000);
        needRecheck = false;
    }while(needRecheck);

    /* RFID Checking */
    do{
        display("Cek RFID", 3, 0, true, false, true);
        delay(400);
        display("o", 2, 0, false, true, false);
        delay(400);
        display("o", 13, 0, false, true, false);
        delay(900); 
        SPI.begin();
        rfid.PCD_Init();
        status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(rfid.uid));
        delay(4);
        if (status != MFRC522::STATUS_OK){
                display("Sukses", 5, 1, false, true, false);
            needRecheck = false;
        }else{
                display("Gagal", 5, 1, false, true, false);
            needRecheck = true;
            delay(1100);
            display("Mangga Direstart", 0, 0, true, false, false);
                display("Atanapi", 2, 1, false, true, true);
            delay(700);
                display("o", 3, 1, false, true, false);
            delay(400);
                display("o", 13, 1, false, true, false);
            delay(900); 
            display("Dicek", 1, 0, true, false, true);
            display("o", 3, 0, false, true, false);
            delay(400);
            display("o", 12, 0, false, true, false);
            delay(400); 
                display("Sambunganna", 4, 1, false, true, true);
                display("o", 1, 1, false, true, false);
            delay(400);
                display("o", 15, 1, false, true, false);
            delay(1000); 
        }
        delay(1000);
    }while(needRecheck);

    /* Loadcell Checking */
    do{
        display("Cek Loadcell", 2, 0, true, false, false);
        delay(400);
        display("o", 0, 0, false, true, false);
        delay(400);
        display("o", 15, 0, false, true, false);
        delay(1000);
        scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
        scale.set_scale(calibration_factor);
        scale.tare();
        delay(400);
        if (scale.is_ready()){
                display("Sukses", 5, 1, false, true, false);
            needRecheck = false;
        }else{
                display("Gagal", 5, 1, false, true, false);
            needRecheck = true;
            delay(1100);
            display("Mangga Direstart", 0, 0, true, false, false);
                display("Atanapi", 2, 1, false, true, true);
            delay(700);
                display("o", 3, 1, false, true, false);
            delay(400);
                display("o", 13, 1, false, true, false);
            delay(900); 
            display("Dicek", 1, 0, true, false, true);
            display("o", 3, 0, false, true, false);
            delay(400);
            display("o", 12, 0, false, true, false);
            delay(400); 
                display("Sambunganna", 4, 1, false, true, true);
                display("o", 1, 1, false, true, false);
            delay(400);
                display("o", 15, 1, false, true, false);
            delay(1000); 
        }
        delay(1000);
    }while(needRecheck);

    ConnectWifi();
    
}

void loop(){
    if (WiFi.status() != WL_CONNECTED) {
    ConnectWifi();
  }

  CekIdKartu();
}