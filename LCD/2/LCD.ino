/*
* NodeMCU I2C 16x2 LCD
* Created by TheCircuit
*/

#include <Wire.h>  // This library is already built in to the Arduino IDE
#include <LiquidCrystal_I2C.h> //This library you can add via Include Library > Manage Library > 
LiquidCrystal_I2C lcd(0x3F, 16, 2);

void setup()
{
  Wire.begin(2,0);
  lcd.init();   // initializing the LCD
  lcd.backlight(); // Enable or Turn On the backlight 
  lcd.print(" Hello Makers "); // Start Printing
}

void loop()
{
  // Nothing Absolutely Nothing!
}
