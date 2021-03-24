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

/* Pin Declaration */
#define RFID_RST_PIN D4
#define RFID_SDA_PIN D3
#define LOADCELL_DOUT_PIN D0
#define LOADCELL_SCK_PIN D8

/* Global Variable Declaration */

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
String id = "";

/* ------------------------------------ */
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


//kalau masuk opsi cek ID
// void is_TimbangOrganik() {
//     display("Kamu memilih opsi menimbang sampah organik");
//     delay(400);
//     if(scale.is_ready()){
//         scale.power_up();
//         scale.tare();
//         display("Mangga tos tiasa di timbang");

//         display("Siap nimbang pencet tombol");


//         do {
//       weight = float(scale.get_units(10) * -453.59237);
//       Serial.print("Weight : ");
//       Serial.print(weight,4);
//       weight = float(weight*37.10259532);
//       Serial.print("Weight : ");
//       Serial.print(weight,4);
//       Serial.println(" g");
//       Serial.print(" calibration_factor: ");
//       Serial.println(calibration_factor);
//       if (weight <= 0.1) {
//         weight = 0.0;
//       }
//       display(String(weight) + " g", 0, 0, true, true, true);
//       display("Nimbang", 5, 1, false, true, true);
//       delay(250);
//       display("x", 3, 1, false, true, false);
//       display("x", 13, 1, false, true, false);
//       delay(250);
//       buttonState = analogRead(0);
//       Serial.println(buttonState);
//       if (times) {
//         lastnumb = buttonState;
//         times++;
//         Serial.println(times);
//         avg_result = weight + avg_result;
//       } else {
//         lastnumb = buttonState;
//         times = 0;
//         usedMode = true;
//         avg_result = 0;
//       }
//       if (times > 5) {
//         avg_result = avg_result/times;
//         Serial.println(avg_result);
//         times = 0;
//         usedMode = false;
//       }

//       delay(100);
//       weight = 0.0;
//       scale.power_down();                   
//       delay(500);
//       scale.power_up();
//     } while (usedMode);
//     display("Sakedap di etang heula");
//     // (Penghitungan berat)
//     display(String(avg_result) + " g", 0, 0, true, false, true);
//     display("Hasil Nimbang", 0, 1, false, true, true);
//     delay(1000);
    
//     }
// }

// Menekan tombol
int isTekan(int buttonstate){// kalau nekannya lama berarti pengguna milih opsinya dan akan mereturn true
    if(buttonState>500){
        times++;
    }

    if (times > 5) {
        times = 0;
        return 1;
    }else if(times<3 && times>1){
        times = 0;
    }
    return true;
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
        bool result = rfid.PCD_PerformSelfTest(); // perform the test
        delay(4);
        if (result){
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

void loop() {
    do{
        boolean isGanti = true;
        // Titik mulai memasukki menu utama
        display("Silahkan Untuk", 1, 0, true, false, true);
        delay(400);
            display("Memilih Opsi", 6, 1, false, true, true);
        delay(400);
            display("o", 0, 1, false, true, false);
        delay(900);
            display("o", 15, 1, false, true, false);
        delay(1000);
            
        //opsi 1
        display("Cek ID Kartu", 1, 0, true, false, true);
        delay(700);
            display("Tekan tombol", 3, 1, false, true, true);
        delay(400);
            display("o", 0, 1, false, true, false);
        delay(900);
            display("o", 15, 1, false, true, false);
        delay(1000);
        do{
          currentState = ((analogRead(A0)>500) ? HIGH : LOW);
          delay(100);
          if(lastState == LOW && currentState == HIGH) {        // button is pressed
            pressedTime = millis();
            isPressing = true;
            isLongDetected = false;

          } else if(lastState == HIGH && currentState == LOW) { // button is released
            isPressing = false;
            releasedTime = millis();

            long pressDuration = releasedTime - pressedTime;

            if( pressDuration < SHORT_PRESS_TIME )
              Serial.println("Ganti Opsi");
              isGanti = false;
          }

          if(isPressing == true && isLongDetected == false) {
            long pressDuration = millis() - pressedTime;

            //Serial.print("\n pressedTime 3 ");
            //Serial.print(pressDuration);
            if( pressDuration > LONG_PRESS_TIME ) {
              Serial.println("Cek ID Berhasil");
              isLongDetected = true;
            }
          }
          lastState = currentState; // save the the last state
        }while(isGanti);

        //opsi 2
        isGanti = true;
        display("Cek Saldo", 1, 0, true, false, true);
        delay(700);
            display("Tekan tombol", 3, 1, false, true, true);
        delay(400);
            display("o", 0, 1, false, true, false);
        delay(900);
            display("o", 15, 1, false, true, false);
        delay(1000);

        do{
          currentState = ((analogRead(A0)>500) ? HIGH : LOW);
          delay(100);
          if(lastState == LOW && currentState == HIGH) {        // button is pressed
            pressedTime = millis();
            isPressing = true;
            isLongDetected = false;

          } else if(lastState == HIGH && currentState == LOW) { // button is released
            isPressing = false;
            releasedTime = millis();

            long pressDuration = releasedTime - pressedTime;

            if( pressDuration < SHORT_PRESS_TIME )
              Serial.println("Ganti Opsi");
              isGanti = false;
          }

          if(isPressing == true && isLongDetected == false) {
            long pressDuration = millis() - pressedTime;

            //Serial.print("\n pressedTime 3 ");
            //Serial.print(pressDuration);
            if( pressDuration > LONG_PRESS_TIME ) {
              Serial.println("Cek Saldo Berhasil");
              isLongDetected = true;
            }
          }
          lastState = currentState; // save the the last state
        }while(isGanti);
        
        //opsi 3
        isGanti = true;
        display("Menimbang Sampah", 0, 0, true, false, false);
        delay(700);
            display("Tekan tombol", 3, 1, false, true, true);
        delay(400);
            display("o", 0, 1, false, true, false);
        delay(900);
            display("o", 15, 1, false, true, false);
        delay(1000);

        do{
          currentState = ((analogRead(A0)>500) ? HIGH : LOW);
          delay(100);
          if(lastState == LOW && currentState == HIGH) {        // button is pressed
            pressedTime = millis();
            isPressing = true;
            isLongDetected = false;

          } else if(lastState == HIGH && currentState == LOW) { // button is released
            isPressing = false;
            releasedTime = millis();

            long pressDuration = releasedTime - pressedTime;

            if( pressDuration < SHORT_PRESS_TIME )
              Serial.println("Ganti Opsi");
              isGanti = false;
          }

          if(isPressing == true && isLongDetected == false) {
            long pressDuration = millis() - pressedTime;

            //Serial.print("\n pressedTime 3 ");
            //Serial.print(pressDuration);
            if( pressDuration > LONG_PRESS_TIME ) {
              Serial.println("Menimbang Sampah Berhasil");
              isLongDetected = true;
            }
          }
          lastState = currentState; // save the the last state
        }while(isGanti);

        //opsi 4
        isGanti = true;
        display("Menimbang Kompos", 0, 0, true, false, false);
        delay(700);
            display("Tekan tombol", 3, 1, false, true, true);
        delay(400);
            display("o", 0, 1, false, true, false);
        delay(900);
            display("o", 15, 1, false, true, false);
        delay(1000);

        do{
          currentState = ((analogRead(A0)>500) ? HIGH : LOW);
          delay(100);
          if(lastState == LOW && currentState == HIGH) {        // button is pressed
            pressedTime = millis();
            isPressing = true;
            isLongDetected = false;

          } else if(lastState == HIGH && currentState == LOW) { // button is released
            isPressing = false;
            releasedTime = millis();

            long pressDuration = releasedTime - pressedTime;

            if( pressDuration < SHORT_PRESS_TIME )
              Serial.println("Ganti Opsi");
              isGanti = false;
          }

          if(isPressing == true && isLongDetected == false) {
            long pressDuration = millis() - pressedTime;

            //Serial.print("\n pressedTime 3 ");
            //Serial.print(pressDuration);
            if( pressDuration > LONG_PRESS_TIME ) {
              Serial.println("Menimbang Kompos Berhasil");
              isLongDetected = true;
            }
          }
          lastState = currentState; // save the the last state
        }while(isGanti);

        //jika tombol lama belum dilakukan maka proses akan kembali ke titik memulai memasuki menu utama
    // }while(isSelesai); 
    }while(true);
}