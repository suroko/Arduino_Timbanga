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

/* Shortcut
1. 20-60 : Pin Declaration and Global Variable Declaration
2. 
*/






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
    display("Konek", 5, 0, false, true, false);
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

/* ------------------------------------ */
void Init(){
    Serial.begin(115200);
    delay(100);
    pinMode(A0, INPUT_PULLUP);
    bool needRecheck = false;

    /* LCD Checking */
    do{
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
            delay(500);
            display(" Mangga Direstart ", 2, 0, true, false, true);
            display(" Atanapi ", 2, 1, true, false, true);
            delay(250);
            display("o", 2, 1, false, true, false);
            display("o", 12, 1, false, true, false);
            delay(1200);
            display("Dicek", 1, 0, true, false, false);
            display("Sambunganna", 4, 1, false, true, true);
            delay(250);
            display("o", 3, 0, false, true, false);
            display("o", 12, 0, false, true, false);
            delay(1200);
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
        if (scale.is_ready()){
            display("Sukses", 5, 1, false, true, false);
            needRecheck = false;
        }else{
            display("Gagal", 5, 1, false, true, false);
            needRecheck = true;
            delay(500);
            display("Mangga Direstart", 2, 0, true, false, true);
            display("Atanapi", 2, 1, true, false, true);
            delay(250);
            display("o", 2, 1, false, true, false);
            display("o", 12, 1, false, true, false);
            delay(1200);
            display("Dicek", 1, 0, true, false, false);
            display("Sambunganna", 4, 1, false, true, true);
            delay(250);
            display("o", 3, 0, false, true, false);
            display("o", 12, 0, false, true, false);
            delay(1200);
        }
        delay(1000);
    }while(needRecheck);

}

/*  Function otentikasi
 *   Require : RFID
 */

/* ------------------------------------ */
bool authentication(String content) {
  String cid = content.substring(0);
  String url = "/" + projectName + "/Authentication.php?card=" + String(cid);
  Serial.print("\nRequesting URL: ");
  Serial.println(url);
  WiFiClient client;
  // client.stop();
  // Use WiFiClient class to create TCP connections
  if (!client.connect(host, 80)) {
    Serial.println("connection failed");
    delay(5000);
    return false;
  }

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
    "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      delay(6000);
      return false;
    }
  }

  Serial.println("receiving from remote server");

  // Check HTTP status
  char status[32] = {
    0
  };
  client.readBytesUntil('\r', status, sizeof(status));
  // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
  if (strcmp(status + 9, "200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    Serial.print(F("Invalid response"));
    display("Punten di", 2, 0, true, false, true);
    delay(400);
    display("o", 1, 0, false, true, false);
    delay(400);
    display("o", 14, 0, false, true, false);
    delay(900);
    display("Tapping ulang", 6, 1, false, true, false);
    delay(1000);
    return false;
  }

  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.print(F("Invalid response"));
    display("Punten di", 2, 0, true, false, true);
    delay(400);
    display("o", 1, 0, false, true, false);
    delay(400);
    display("o", 14, 0, false, true, false);
    delay(900);
    display("Tapping ulang", 6, 1, false, true, false);
    delay(1000);
    return false;
  };

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
  // DeserializationError error = deserializeMsgPack(doc, s);

  // if (error) {
  //   Serial.print("deserializeMsgPack() failed: ");
  //   Serial.println(error.f_str());
  //   return false;
  // }
  deserializeJson(doc, s);

  JsonObject object = doc.as < JsonObject > ();
  const char * temp = object["username"].as <
    const char * > ();

  if (String(temp) != "") {
    display("o Wilujeng o", 3, 0, true, false, true);
    idTemp = String(temp);
    int length = idTemp.length() - 1;
    int pad = (length >= 16) ? 0 : (16 - length) / 2;
    Serial.println("Panjang Nama : " + pad);
    display(idTemp, pad, 1, false, true, false);
    
    temp = object[String("id")];
    idTemp = String(temp);
    usedMode = true;
    Serial.println(usedMode);
    delay(1500);
    return true;
  } else {
    display("Punten, Kartuna", 1, 0, true, false, false);
    display("Teu Kadaptar", 1, 1, false, true, false);
    for (int i = 0; i < 3; i++) {
      display(".", 1, 1, false, true, false);
      delay(800);
    }
    return false;
  }

}

void Otentikasi(){
    Serial.println("\n Input your card to the reader...");
    delay(300);

    display(" Sampurasun ", 2, 0, true, false, true);
    delay(250);
    display("o", 1, 0, false, true, false);
    display("o", 14, 0, false, true, false);
    delay(1200);
    display("Mangga Kartuna", 1, 0, true, false, false);
    display("di Tap", 4, 1, false, true, true);
    delay(250);
    display("o", 3, 1, false, true, false);
    display("o", 12, 1, false, true, false);
    delay(1200);
    standbyMode = true;

    if (!rfid.PICC_IsNewCardPresent())
        return;

    if (!rfid.PICC_ReadCardSerial())
        return;
    
    display("o Sakedap di o", 2, 0, true, false, true);
    display("parios heula", 1, 1, false, true, true);
    delay(250);
    display("o", 0, 1, false, true, false);
    display("o", 15, 1, false, true, false);
    delay(1200);
    Serial.print("UID tag :");
    String content = "";

    for (byte i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
        Serial.print(rfid.uid.uidByte[i], HEX);

        content.concat(String(rfid.uid.uidByte[i] < 0x10 ? "0" : ""));
        content.concat(String(rfid.uid.uidByte[i], HEX));
    }
    standbyMode = false;
    delay(1500);
    content.toUpperCase();
    if (!authentication(content)) {
        return;
    }
}

/* ------------------------------------ */
void scaling() {  
  if (scale.is_ready()) {
    scale.set_scale(calibration_factor);
    scale.tare(); //Reset the scale to 0
    display("Mangga tos tiasa", 0, 0, true, false, false);
    display("di Timbang", 2, 1, false, true, false);
    for (int i = 0; i < 3; i++) {
      display(".", 2, 1, false, true, false);
      delay(900);
    }
    do {
      weight = float(scale.get_units(10) * -453.59237);
      Serial.print("Weight : ");
      Serial.print(weight,4);
      weight = float(weight*37.10259532);
      Serial.print("Weight : ");
      Serial.print(weight,4);
      Serial.println(" g");
      Serial.print(" calibration_factor: ");
      Serial.println(calibration_factor);
      if (weight <= 0.1) {
        weight = 0.0;
      }
      display(String(weight) + " g", 0, 0, true, true, true);
      display("Nimbang", 5, 1, false, true, true);
      delay(250);
      display("x", 3, 1, false, true, false);
      display("x", 13, 1, false, true, false);
      delay(250);
      buttonState = analogRead(0);
      Serial.println(buttonState);
      if (times) {
        lastnumb = buttonState;
        times++;
        Serial.println(times);
        avg_result = weight + avg_result;
      } else {
        lastnumb = buttonState;
        times = 0;
        usedMode = true;
        avg_result = 0;
      }
      if (times > 5) {
        avg_result = avg_result/times;
        Serial.println(avg_result);
        times = 0;
        usedMode = false;
      }

      delay(100);
      weight = 0.0;
      scale.power_down();                   
      delay(500);
      scale.power_up();
    } while (usedMode);
    display(String(avg_result) + " g", 0, 0, true, false, true);
    display("Hasil Nimbang", 0, 1, false, true, true);
    delay(1000);
  } else {
    display("Sensor Timbangan", 0, 0, true, false, false);
    display("Teu Kadeteksi", 0, 1, false, true, false);
    Serial.println(" HX711 not found.");
  }
}

/* ------------------------------------ */

/*  Function Mengirim data timbangan ke server
 *   Require : ID user
 */
int sendData(String id) {
  address.replace(" ", "%20");
  String url = "/" + projectName + "/record.php?id=" + id + "&w=" + avg_result + "&address=" + address;
  avg_result = 0;
  Serial.print("\nRequesting URL: ");
  Serial.println(url);
  WiFiClient client;
  // Use WiFiClient class to create TCP connections
  if (!client.connect(host, 80)) {
    Serial.println("connection failed");
    delay(5000);
    return 2;
  }

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
    "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      delay(6000);
      return 3;
    }
  }

  Serial.println("receiving from remote server");

  // Check HTTP status
  char status[32] = {
    0
  };
  client.readBytesUntil('\r', status, sizeof(status));
  // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
  if (strcmp(status + 9, "200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    return 4;
  }

  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.print(F("Invalid response"));
    return 5;
  };

  String s;
  char ch;
  if (client.find("{")) {
    s.concat("{");
    while (client.available() && String(ch) != "}") {
      ch = static_cast < char > (client.read());
      s.concat(ch);
    }
  }
  Serial.println(s);
  const size_t CAPACITY = JSON_OBJECT_SIZE(2);
  StaticJsonDocument < CAPACITY > doc;
  deserializeJson(doc, s);

  JsonObject object = doc.as < JsonObject > ();
  int temp = object["success"];
  Serial.print(temp);
  delay(1500);
  Serial.print(s);
  if (temp > 0) {
    return 1;
  } else {
    return 6;
  }
}

void setup(){
    init();
    ConnectWifi();
}


void loop(){
    
  // if (WiFi.status() != WL_CONNECTED) {
  //   ConnectWifi();
  //   }
  //   Otentikasi();
  //   scaling();

  //   int x = sendData(idTemp);
  // switch (x) {
  // case 1:
  //   display("Data sukses", 2, 0, true, false, true);
  //   delay(400);
  //   display("o", 1, 0, false, true, false);
  //   delay(400);
  //   display("o", 15, 0, false, true, false);
  //   delay(400);
  //   display("Kasimpen", 6, 1, false, true, true);
  //   display("o", 2, 1, false, true, false);
  //   delay(400);
  //   display("o", 13, 1, false, true, false);
  //   delay(1000);
    
  //   display("Hatur Nuhun", 2, 0, true, false, true);
  //   delay(400);
  //   display("o", 1, 0, false, true, false);
  //   delay(400);
  //   display("o", 15, 0, false, true, false);
  //   delay(1300);
    
  //   break;

  // case 2: //Connection Failed
  //   display("Nyimpen data", 1, 0, true, false, true);
  //   display("Gagal sd002", 2, 1, false, true, true);
  //   delay(1500);
  //   display("Mangga di", 2, 0, true, false, true);
  //   delay(400);
  //   display("o", 1, 0, false, true, false);
  //   delay(400);
  //   display("o", 14, 0, false, true, false);
  //   delay(900);
  //   display("Cobi ulang", 6, 1, false, true, false);
  //   delay(1000);
  //   break;

  // case 3: //Client Timeout
  //   display("Nyimpen data", 1, 0, true, false, true);
  //   display("Gagal sd003", 2, 1, false, true, true);
  //   delay(1500);
  //   display("Mangga di", 2, 0, true, false, true);
  //   delay(400);
  //   display("o", 1, 0, false, true, false);
  //   delay(400);
  //   display("o", 14, 0, false, true, false);
  //   delay(900);
  //   display("Cobi ulang", 6, 1, false, true, false);
  //   delay(1000);
  //   break;

  // case 4: //Unexpected Response
  //   display("Nyimpen data", 1, 0, true, false, true);
  //   display("Gagal sd004", 2, 1, false, true, true);
  //   delay(1500);
  //   display("Mangga di", 2, 0, true, false, true);
  //   delay(400);
  //   display("o", 1, 0, false, true, false);
  //   delay(400);
  //   display("o", 14, 0, false, true, false);
  //   delay(900);
  //   display("Cobi ulang", 6, 1, false, true, false);
  //   delay(1000);
  //   break;

  // case 5: //Invalid Response
  //   display("Nyimpen data", 1, 0, true, false, true);
  //   display("Gagal sd005", 2, 1, false, true, true);
  //   delay(1500);
  //   display("Mangga di", 2, 0, true, false, true);
  //   delay(400);
  //   display("o", 1, 0, false, true, false);
  //   delay(400);
  //   display("o", 14, 0, false, true, false);
  //   delay(900);
  //   display("Cobi ulang", 6, 1, false, true, false);
  //   delay(1000);
  //   break;

  // case 6: //Data Inputed Failed
  //   display("Nyimpen data", 1, 0, true, false, false);
  //   display("Gagal sd006", 2, 1, false, true, true);
  //   delay(1500);
  //   display("Mangga di", 2, 0, true, false, true);
  //   delay(400);
  //   display("o", 1, 0, false, true, false);
  //   delay(400);
  //   display("o", 14, 0, false, true, false);
  //   delay(900);
  //   display("Cobi ulang", 6, 1, false, true, false);
  //   delay(1000);
  //   break;

  // default: //Error Unknown
  //   display("Nyimpen data", 1, 0, true, false, false);
  //   display("Gagal sdxxx", 2, 1, false, true, true);
  //   delay(1500);
  //   display("Mangga di", 2, 0, true, false, true);
  //   delay(400);
  //   display("o", 1, 0, false, true, false);
  //   delay(400);
  //   display("o", 14, 0, false, true, false);
  //   delay(900);
  //   display("Cobi ulang", 6, 1, false, true, false);
  //   delay(1000);
  //   break;
  // }
}