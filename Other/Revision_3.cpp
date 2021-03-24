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
  #define LOADCELL_DOUT_PIN D0
  #define LOADCELL_SCK_PIN D8

  // scale
  // scale
  HX711 scale;
  float calibration_factor = -244.0;
  float weight;
  float Weight = 0;

  const char* ssid = "Kudu bisa!~"; //WiFi SSID
  const char* password = "pasti bisa"; //WiFi Password
  const char* host = "192.168.43.76"; //Host 

  String condition = "";
  int lastCln;
  int lastRow;
  String idTemp = "";
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

      //2. Prepare Scale
    display("Cek Loadcell",0,0,true,false);
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale();
    scale.tare();
    for (int i = 0; i < 3; i++)
    {
      display(".",0,0,false,true);
      delay(1000);
    }
    display("Berhasil",4,1,false,true);
    delay(1500);

    ConnectWifi();
  }

  void authentication(String content){
    String cid = content.substring(0);
    String url = "/projectkeren/Authentication.php?card=" + String(cid);
    Serial.print("\nRequesting URL: ");
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
        delay(6000);
        return;
      }
    }

    Serial.println("receiving from remote server");

    // Check HTTP status
    char status[32] = {0};
    client.readBytesUntil('\r', status, sizeof(status));
    // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
    if (strcmp(status + 9, "200 OK") != 0) {
      Serial.print(F("Unexpected response: "));
      Serial.println(status);
      return;
    }

    char endOfHeaders[] = "\r\n\r\n";
    if (!client.find(endOfHeaders)){
      Serial.print(F("Invalid response"));
      return;
    };

    const size_t CAPACITY = JSON_OBJECT_SIZE(4);
    StaticJsonDocument<CAPACITY> doc;
    String s;
      while (client.available()) {
        char ch = static_cast < char > (client.read());
        s.concat(ch); 
        
      }
      deserializeJson(doc, s);

      JsonObject object = doc.as<JsonObject>();
      const char* temp = object["username"];
      Serial.print(temp);
      if(temp >0){
        display("Hello ",1,0,true,false);
        display(temp,1,0,false,true);
        temp = object["id"];
        idTemp = String(temp);
        
        }else{
          display("Card Id",2,0,true,false);
          display("Not found",1,1,false,true);
        for (int i = 0; i < 3; i++) {
          display(".",3,1,false,true);
          delay(1000);
        }
      }
      
      delay(1500);
      Serial.print(s);
  }


  void sendData(String id){
    String address = "Jl. Abdinegara";
    address.replace(" ", "%20");
    String url = "/projectkeren/record.php?id=" + id + "&w=" + Weight + "&address=" + address ;
    Serial.print("\nRequesting URL: ");
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
        delay(6000);
        return;
      }
    }

    Serial.println("receiving from remote server");

    // Check HTTP status
    char status[32] = {0};
    client.readBytesUntil('\r', status, sizeof(status));
    // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
    if (strcmp(status + 9, "200 OK") != 0) {
      Serial.print(F("Unexpected response: "));
      Serial.println(status);
      return;
    }

    char endOfHeaders[] = "\r\n\r\n";
    if (!client.find(endOfHeaders)){
      Serial.print(F("Invalid response"));
      return;
    };

    const size_t CAPACITY = JSON_OBJECT_SIZE(2);
    StaticJsonDocument<CAPACITY> doc;
    String s;
      while (client.available()) {
        char ch = static_cast < char > (client.read());
        s.concat(ch); 
      }
      deserializeJson(doc, s);

      JsonObject object = doc.as<JsonObject>();
      int temp = object["success"];
      Serial.print(temp);
      if(temp >0){
        display("Data Inputed",0,0,true,false);
        for (int i = 0; i < 3; i++) {
          display(".",0,0,false,true);
          delay(1000);
        }
        display("successful",4,1,false,true);
        }else{
          display("Data Inputed",2,0,true,false);
          display("failed",4,1,false,true);
        }
      delay(1500);
      Serial.print(s);
  }

  void scaling(){
    if (scale.is_ready()) {
        scale.set_scale(calibration_factor);
        condition = "";
        while(condition.isEmpty()){
        weight = scale.get_units();
        Serial.print("Weight : ");
        
        if (weight <= 0.1) {
          weight = 0.0;
        }
        Weight = weight * 100;
        delay(100);
        display("Weight : ",0,0,true,false);
        display(String(Weight)+" g",0,0,false,true);
        Serial.println(weight *= 100);

        Serial.println("type anything to stop weight ");
        condition = Serial.readString();
        delay(1000);
        weight = 0.0;
        }
      } else {
        Serial.println(" HX711 not found.");
      }
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
    scaling();
    sendData(idTemp);
  }

