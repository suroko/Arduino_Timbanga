/* Ref 0.9 : 26 Feb 2021 NO RFID*/
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

  // PIN RFI D 
  #define RST_PIN D8
#define SDA_PIN D0

// PINo HX711 circuit wiring
#define LOADCELL_DOUT_PIN D3
#define LOADCELL_SCK_PIN D4

// scale
HX711 scale;
float calibration_factor = 21.41;
float weight;
float Weight = 0;

const char * ssid = "Kudu bisa!~"; //WiFi SSID
const char * password = "pasti bisa"; //WiFi Password
const char * host = "tempatsampah.online"; //Host 
const String projectName = "ArduinoKelurahan";
String address = "Jl. Abdinegara";
int buttonState = 0;
int lastnumb;
int times = 0;

String condition = "";
int lastCln;
int lastRow;
String idTemp = "";
bool standbyMode = false;
bool usedMode = false;

// RFID
MFRC522 rfid(SDA_PIN, RST_PIN);

LiquidCrystal_I2C lcd(0x27, 16, 2);

/*  Function Menampilkan pesan pada LCD
 *   Dengan inputan massage: isi pesan, cln: posisi kolom, row: posisi baris, 
 *   isClear: Membersihkan semua pesan? , isCont: Melanjutkan pesan?,
 *   isCenter: Menengahkan tulisan.
 */
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

/*  Function Menghubungkan alat pada wifi
 *   Jika alat sudah terhubung kepada wifi maka stanbyMode akan aktif
 */
void ConnectWifi() {

  Serial.print("\nConnecting to \n" + String(ssid));

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("\nWaiting to be connected...");
    display("Wifi ", 0, 0, true, false, false);
    display(ssid, 0, 0, false, true, false);
    display("Connecting", 1, 1, false, true, false);
    for (int i = 0; i < 3; i++) {
      display(".", 1, 1, false, true, false);
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

//  Setup Function
void setup() {
  Serial.begin(115200);
  delay(100);
  pinMode(A0, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  display("Cek lcd", 3, 0, true, false, false);
  for (int i = 0; i < 3; i++) {
    display(".", 3, 0, false, true, false);
    delay(1000);
  }
  display("Sukses", 5, 1, false, true, false);
  delay(1500);

  display("Cek RFID", 3, 0, true, false, false);
  for (int i = 0; i < 3; i++) {
    display(".", 3, 0, false, true, false);
    delay(1000);
  }
  SPI.begin();
  rfid.PCD_Init();
  display("Sukses", 5, 1, false, true, false);
  delay(1500);

  display("Cek Loadcell", 0, 0, true, false, false);
  for (int i = 0; i < 3; i++) {
    display(".", 0, 0, false, true, false);
    delay(1000);
  }
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(2280.f);
  scale.tare();
  display("Sukses", 5, 1, false, true, false);
  delay(1500);

  ConnectWifi();
}

/*  Function otentikasi
 *   Require : RFID
 */
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
    return false;
  }

  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.print(F("Invalid response"));
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
    display("Sampurasun ", 3, 0, true, false, false);
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
      delay(1000);
    }
    return false;
  }

}

/*  Function Mengirim data timbangan ke server
 *   Require : ID user
 */
int sendData(String id) {
  address.replace(" ", "%20");
  String url = "/" + projectName + "/record.php?id=" + id + "&w=" + Weight + "&address=" + address;
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

/* Function Menimbang
 *   Timbangan akan terus berjalan hingga tombol button ditekan selama 3-5 detik
 */
void scaling() {

  if (scale.is_ready()) {
    scale.set_scale(2280.f);
    scale.tare(); //Reset the scale to 0
    display("Mangga tos tiasa", 0, 0, true, false, false);
    display("di Timbang", 2, 1, false, true, false);
    for (int i = 0; i < 3; i++) {
      display(".", 2, 1, false, true, false);
      delay(1500);
    }
    do {
      weight = scale.get_units();
      Serial.print("\nWeight : ");

      if (weight <= 0.1) {
        weight = 0.0;
      }
      Weight = weight;
      delay(100);
      display("Beuratna : ", 0, 0, true, false, true);
      display(String(weight) + " g", 0, 1, false, true, true);
      Serial.println(weight *= 100);
      delay(250);
      buttonState = analogRead(0);
      Serial.println(buttonState);
      if (buttonState > 800) {
        lastnumb = buttonState;
        times++;
        Serial.println(times);
      } else {
        lastnumb = buttonState;
        times = 0;
        usedMode = true;
      }

      if (times > 1) {
        times = 0;
        usedMode = false;
      }

      delay(1000);
      weight = 0.0;
      scale.power_down();                   
    delay(500);
    scale.power_up();
    } while (usedMode);
  } else {
    display("Sensor Timbangan", 0, 0, true, false, false);
    display("Teu Kadeteksi", 0, 1, false, true, false);
    Serial.println(" HX711 not found.");
  }
}

// Function Looping
void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    ConnectWifi();
  }

  Serial.println("\n Input your card to the reader...");
  delay(1000);
  if (!standbyMode) {
    standbyMode = true;
  }

  standbyMode = false;
  
  if (false) {//!authentication(content)
    return;
  }
  scaling();

  int x = sendData(idTemp);
  switch (x) {
  case 1:
    display("Data Berhasil", 1, 0, true, false, false);
    display("Tersimpan", 4, 1, false, true, false);
    for (int i = 0; i < 3; i++) {
      display(".", 4, 1, false, true, false);
      delay(500);
    }
    break;

  case 2: //Connection Failed
    display("Nyimpen data", 1, 0, true, false, false);
    display("Gagal sd002", 2, 1, false, true, true);
    delay(1500);
    break;

  case 3: //Client Timeout
    display("Nyimpen data", 1, 0, true, false, false);
    display("Gagal sd003", 2, 1, false, true, true);
    delay(1500);
    break;

  case 4: //Unexpected Response
    display("Nyimpen data", 1, 0, true, false, false);
    display("Gagal sd004", 2, 1, false, true, true);
    delay(1500);
    break;

  case 5: //Invalid Response
    display("Nyimpen data", 1, 0, true, false, false);
    display("Gagal sd005", 2, 1, false, true, true);
    delay(1500);
    break;

  case 6: //Data Inputed Failed
    display("Nyimpen data", 1, 0, true, false, false);
    display("Gagal sd006", 2, 1, false, true, true);
    delay(1500);

  default: //Error Unknown
    display("Nyimpen data", 1, 0, true, false, false);
    display("Gagal sdxxx", 2, 1, false, true, true);
    delay(1500);
    break;
  }

}