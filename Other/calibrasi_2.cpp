// 1.1 mengganti authentication function menjadi boolean dan returnnya
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
#define RST_PIN D9
#define SDA_PIN D8

// PINo HX711 circuit wiring
#define LOADCELL_DOUT_PIN D5
#define LOADCELL_SCK_PIN D4

// scale
HX711 scale;
float calibration_factor = -10;
float weight = 0.0;


#define NUM_MEASUREMENTS 20 // Number of measurements
#define THRESHOLD 2			// Measures only if the weight is greater than 2 kg. Convert this value to pounds if you're not using SI units.
#define THRESHOLD1 0.5  // Restart averaging if the weight changes more than 0.5 kg.

const char * ssid = "Kudu bisa!~"; //WiFi SSID
const char * password = "pasti bisa"; //WiFi Password
const char * host = "192.168.43.76"; //Host 
const String projectName = "projectsekolah";
String address = "Jl. Abdinegara";
int a = 0;
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
*   isClear: Membersihkan semua pesan? , isCont: Melanjutkan pesan?
*/

float units;
float ounces;

void setup(){
  Serial.begin(115200);
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare();  //Reset the scale to 0
  

  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
  
}

void loop(){
  if (scale.is_ready()) {
    scale.set_scale(calibration_factor); //Adjust to this calibration factor
    Serial.print("Reading: ");
    units = scale.get_units();
    if (units < 0)
    {
      units = 0.00;
    }
    ounces = units * 0.035274;
    delay(100);
    //units *= 100;
    Serial.print(units);
    Serial.print(" grams"); 
    Serial.print(" calibration_factor: ");
    Serial.print(calibration_factor);
    Serial.println();
    delay(250);
    units = 0.0;
    if(Serial.available())
    {
      char temp = Serial.read();
      if(temp == '+' || temp == 'a')
        calibration_factor += 1;
      else if(temp == '-' || temp == 'z')
        calibration_factor -= 1;
      else if(temp == '-' || temp == 'b')
        calibration_factor += 10;
      else if(temp == '-' || temp == 'y')
        calibration_factor -= 10;
      else if(temp == '-' || temp == 'c')
        calibration_factor += 100;
      else if(temp == '-' || temp == 'x')
        calibration_factor -= 100;
      else if(temp == '-' || temp == 'p')
        calibration_factor += 0.1;
      else if(temp == '-' || temp == 'o')
        calibration_factor -= 0.1;
      else if(temp == '-' || temp == 'i')
        calibration_factor += 0.01;
      else if(temp == '-' || temp == 'u')
        calibration_factor -= 0.01;
      else if(temp == 'd')
         scale.tare();
    }
  }
}
