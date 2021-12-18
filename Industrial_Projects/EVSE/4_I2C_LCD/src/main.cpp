#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

hw_timer_t * timer1 = NULL;
portMUX_TYPE timerMux1 = portMUX_INITIALIZER_UNLOCKED;

#define pinToggle 23

int timerCounter =0; 
volatile uint8_t led1stat = 0;
uint8_t oneSecondFlag =0;

// set the LCD number of columns and rows
int lcdColumns = 20;
int lcdRows = 4;
int a =0;
// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  
String messageToScroll = "Shaival From National Infotech 2021";

void IRAM_ATTR onTimer1(){
  // Critical Code here
  portENTER_CRITICAL_ISR(&timerMux1);
  timerCounter+=1;

  if (timerCounter == 10000)
  {
    /* code */
    timerCounter = 0;
    oneSecondFlag = 1;
  }
  portEXIT_CRITICAL_ISR(&timerMux1);
}

void setup(){
  Serial.begin(115200);
  pinMode(pinToggle, OUTPUT);
  digitalWrite(pinToggle, LOW);
  delay(10);

  Serial.println("Start timer 1");
  timer1 = timerBegin(1, 80, true);  // timer 1, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 80 -> 1000 ns = 1 us, countUp
  timerAttachInterrupt(timer1, &onTimer1, true); // edge (not level) triggered 
  timerAlarmWrite(timer1, 10, true); // 10 * 1 us = 10 us, autoreload true

  // Enable the timer alarms
   timerAlarmEnable(timer1); // enable

  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
}

void loop(){
  // set cursor to first column, first row
  lcd.setCursor(0,0);
  // print message
//  lcd.print(a);
//  Serial.println(a);
//  delay(1000);
//  a+=1;
  
  lcd.print("Hello, World!");
  delay(1000);
  // clears the display to print new message
  lcd.clear();
  
  // set cursor to first column, second row
  lcd.setCursor(0,1);
  lcd.print("Surat");
  delay(1000);
  lcd.clear();

  // set cursor to first column, second row
  lcd.setCursor(0,2);
  lcd.print(a);
  delay(1000);
  lcd.clear();
//
//scrollText(2, messageToScroll, 250, lcdColumns);

  if (oneSecondFlag == 1)
  {
    /* code */
      oneSecondFlag = 0; 
      led1stat=1-led1stat;
      digitalWrite(pinToggle, led1stat);   // turn the LED on or off
  }
   
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