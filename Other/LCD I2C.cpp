void setup()
{
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  Serial.begin(9600);
}

void loop()
{
  // when characters arrive over the serial port...
  lcd.setCursor(3,0);
  lcd.print("ridwan");
}