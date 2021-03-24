#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h> 
#include <WiFiClient.h> 
#include <HX711.h> 
#include <Wire.h> 
#include <LiquidCrystal_I2C.h> 
#include <ArduinoJson.h>

/* Machine Information */
String address = "Jl. Abdinegara";
const char * host = "tempatsampah.online";
const String projectName = "ArduinoKelurahan";

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

void CekSaldo(){
/* Mesin akan menampilkan pilihan pesan "Cek ID Kartu", "Cek Saldo", "Menimbang Sampah Organik", 
* dan "Menimbang Kompos" pada baris pertama di LCD secara bergantian, dengan menggunakan tombol 
* untuk mengganti pilihannya, disertai dengan pesan "Pilih Menu" pada baris kedua
*/

/* Pengguna memilih pilihan "Cek Saldo" dengan menekan tombol selama 2 detik */

    display("Sakedap ", 1, 0, true, false, true);
    delay(400);
    display("o", 2, 0, false, true, false);
    delay(400);
    display("o", 12, 0, false, true, false);
    delay(900);
        display("Di Parios Heula", 0, 1, false, true, false);
    delay(1000);
    
    String cid = id.substring(0);
    String url = "/" + projectName + "/CekSaldo.php?card=" + String(cid);
    WiFiClient client;

    if (!client.connect(host, 80)) {
        Serial.println("connection failed");
        delay(5000);
        
        /* 
        *  Error ga bisa menyambungkan ke server
        *  Cek Koneksi
        */
        display("Cek Saldo", 1, 0, true, false, true);
            display("Gagal cs001", 2, 1, false, true, true);
        delay(1500);
        display("Mangga di", 2, 0, true, false, true);
        delay(400);
        display("o", 1, 0, false, true, false);
        delay(400);
        display("o", 14, 0, false, true, false);
        delay(900);
            display("Cobi ulang", 6, 1, false, true, false);
        delay(1000);
        return;
    }

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
    "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            delay(6000);

            /* 
            *  Error sambungan terputus
            *  Cek Koneksi
            */
            display("Cek Saldo", 1, 0, true, false, true);
                display("Gagal cs002", 2, 1, false, true, true);
            delay(1500);
            display("Mangga di", 2, 0, true, false, true);
            delay(400);
            display("o", 1, 0, false, true, false);
            delay(400);
            display("o", 14, 0, false, true, false);
            delay(900);
                display("Cobi ulang", 6, 1, false, true, false);
            delay(1000);
            return;
        }
    }

    Serial.println("receiving from remote server");

    // Check HTTP status
    char status[32] = {0};
    client.readBytesUntil('\r', status, sizeof(status));

    // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
    if (strcmp(status + 9, "200 OK") != 0) {

        /* 
        *  Error respon tidak valid
        *  Cek alamat website
        */
        Serial.print(F("Unexpected response: "));
        Serial.println(status);
        Serial.print(F("Invalid response"));
        display("Cek Saldo", 1, 0, true, false, true);
                display("Gagal cs003", 2, 1, false, true, true);
        delay(1500);
        display("Mangga di", 2, 0, true, false, true);
        delay(400);
        display("o", 1, 0, false, true, false);
        delay(400);
        display("o", 14, 0, false, true, false);
        delay(900);
            display("Cobi ulang", 6, 1, false, true, false);
        delay(1000);
        return;
    }

    char endOfHeaders[] = "\r\n\r\n";
    if (!client.find(endOfHeaders)) {
        
         /* 
        *  Error respon tidak valid
        *  Cek alamat website
        */
        Serial.print(F("Invalid response"));
        display("Cek Saldo", 1, 0, true, false, true);
                display("Gagal cs004", 2, 1, false, true, true);
        delay(1500);
        display("Mangga di", 2, 0, true, false, true);
        delay(400);
        display("o", 1, 0, false, true, false);
        delay(400);
        display("o", 14, 0, false, true, false);
        delay(900);
            display("Cobi ulang", 6, 1, false, true, false);
        delay(1000);
        return;
    }

    String s;
    char ch;
    if (client.find("{")) {
        s.concat("{");
        while (client.available() && String(ch) != "}") {
            ch = static_cast < char > (client.read());
            // Serial.print(ch);
            s.concat(ch);
        }
    }

    Serial.println(s);
    const size_t CAPACITY = JSON_OBJECT_SIZE(4) + 60;
    DynamicJsonDocument doc(CAPACITY);
    deserializeJson(doc, s);

    JsonObject object = doc.as < JsonObject > ();
    const char * temp = object["saldo"].as <
    const char * > ();

    if (String(temp) != "") {
        display("o Saldo Ayeuna o", 3, 0, true, false, true);
        idTemp = String(temp);
        int length = idTemp.length() - 1;
        int pad = (length >= 16) ? 0 : (16 - length) / 2;
        Serial.println("Panjang Nama : " + pad);
            display(idTemp, pad, 1, false, true, false);
        
        delay(1500);
    } else {
        
    }
}

void setup(){

}

void loop(){

}