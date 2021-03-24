/* Ref 0.9 : 26 Feb 2021*/
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


#define LOADCELL_DOUT_PIN  D0
#define LOADCELL_SCK_PIN  D8

HX711 scale;

float calibration_factor = 9360; //-7050 worked for my 440lb max scale setup
float fix;

void reset();

void setup() {
  reset();
}

void loop() {
  float z = 0;
  for(int i=0;i<4;i++){
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  Serial.print("Reading: ");
  float a = scale.get_units(10)* -1;
  fix = ((a + 1.523) / 0.223);
  z += a;
   if (a < 0 ) {
    a = 0;
    }
  Serial.print(a, 4);
  Serial.print(" lbs "); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  a = a * 453.59237;
  Serial.print(a, 4);
  Serial.print(" g"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  float b = scale.read_average(10); //Get a baseline reading
  Serial.print(" read average: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.print(b, 4);
  Serial.print(" lbs");
  float c = scale.get_value(10); //Get a baseline reading
  Serial.print(" get value: ");
  Serial.print(c, 4);
  Serial.println(" lbs");
  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a')
      calibration_factor += 10;
    else if(temp == '-' || temp == 'z')
      calibration_factor -= 10;
    else if(temp == '-' || temp == 'q')
      calibration_factor += 100;
    else if(temp == '-' || temp == 'w')
      calibration_factor -= 100;
    else if(temp == '-' || temp == 'e')
      calibration_factor += 1000;
    else if(temp == '-' || temp == 'd')
      calibration_factor -= 1000;
    else if(temp == '-' || temp == 'h')
      calibration_factor += 1;
    else if(temp == '-' || temp == '  ')
      calibration_factor -= 1;
    else if(temp == '-' || temp == 'r')
      reset();
    
  }
  }
  Serial.print(" Result: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.print((z/4), 4);
  Serial.println(" lbs");
 scale.power_down();                   
  delay(500);
  scale.power_up();
}

void reset(){
    Serial.begin(115200);
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");
  
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(2280.f);
  scale.tare(); //Reset the scale to 0
  
  float zero_factor = scale.read_average(10); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.print(zero_factor, 4);
  Serial.println(" lbs");
}
