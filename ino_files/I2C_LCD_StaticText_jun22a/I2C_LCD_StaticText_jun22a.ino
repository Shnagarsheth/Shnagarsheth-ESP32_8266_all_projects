#include <LiquidCrystal_I2C.h>

// set the LCD number of columns and rows
int lcdColumns = 20;
int lcdRows = 4;
int a =0;
// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  
String messageToScroll = "Shaival From National Infotech 2021";

void setup(){
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
}

void loop(){
  // set cursor to first column, first row
  lcd.setCursor(0, 0);
  // print message
  lcd.print(a);
  delay(1000);
  a+=1;
  
//  lcd.print("Hello, World!");
//  delay(1000);
//  // clears the display to print new message
//  lcd.clear();
//  
//  // set cursor to first column, second row
//  lcd.setCursor(0,1);
//  lcd.print("Surat");
//  delay(1000);
//  lcd.clear();
//
//  // set cursor to first column, second row
//  lcd.setCursor(0,2);
//  lcd.print(a);
//  delay(1000);
//  lcd.clear();
//
//  scrollText(3, messageToScroll, 250, lcdColumns);
   
}

/* Function to scroll text
 * The function acepts the following arguments:
 * row: row number where the text will be displayed
 * message: message to scroll
 * delayTime: delay between each character shifting
 * lcdColumns: number of columns of your LCD
 */

void scrollText(int row, String message, int delayTime, int lcdColumns) {
  for (int i=0; i < lcdColumns; i++) {
    message = " " + message; 
  } 
  message = message + " "; 
  for (int pos = 0; pos < message.length(); pos++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(pos, pos + lcdColumns));
    delay(delayTime);
  }
}
