#include <Arduino.h>

hw_timer_t * timer1 = NULL;
portMUX_TYPE timerMux1 = portMUX_INITIALIZER_UNLOCKED;

#define LED1 2
#define PWM1 5 //PWM pin 
#define ADC1 39 //Declare AI_pin
#define pushButton 35 //Decalre as Push Button

volatile uint8_t led1stat = 0;
uint8_t flagKeyScan = 0, oneSecondFlag =0, buttonFlag = 0;

// setting PWM properties
const int freq = 5000; //5KHz signal
const int ledChannel = 0; //Need to choose a PWM channel. There are 16 channels from 0 to 15.
const int resolution = 8; // Need to set the signal’s duty cycle resolution: you have resolutions from 1 to 16 bits.
//We’ll use 8-bit resolution, which means you can control the LED brightness using a value from 0 to 255.
int dutyCycle = 128;
int analogValue = 0;
int timerCounter =0; 

void IRAM_ATTR onTimer1(){
  // Critical Code here
  portENTER_CRITICAL_ISR(&timerMux1);
  timerCounter+=1;

  if(timerCounter == 80000){
    flagKeyScan=1;
  }
  if (timerCounter == 100000)
  {
    /* code */
    timerCounter = 0;
    oneSecondFlag = 1;
  }
  

  portEXIT_CRITICAL_ISR(&timerMux1);
}

void setup() {
  Serial.begin(115200);
  pinMode(pushButton, INPUT);
  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, LOW);    // turn the LED off by making the voltage LOW
  delay(10);

  Serial.println("Start timer 1");
  timer1 = timerBegin(1, 80, true);  // timer 1, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 80 -> 1000 ns = 1 us, countUp
  timerAttachInterrupt(timer1, &onTimer1, true); // edge (not level) triggered 
  timerAlarmWrite(timer1, 10, true); // 10 * 1 us = 10 us, autoreload true

  // Enable the timer alarms
   timerAlarmEnable(timer1); // enable

  //COnfiguring PWM functionalities
  ledcSetup(ledChannel, freq, resolution);

  //Attaching channel to the GPIO pin
  ledcAttachPin(PWM1, ledChannel);

}

void loop() {
  static uint8_t x =0;
  ledcWrite(ledChannel,dutyCycle);

  analogValue=analogRead(ADC1);
  Serial.println("Analog Channel is read Successfully");

  buttonFlag = digitalRead(pushButton);
  Serial.println("Button read Complete!!");

  if (buttonFlag == HIGH)
  {
    if (flagKeyScan == 1)
    {
      flagKeyScan =0;
      if (buttonFlag == HIGH)
      {
        if (dutyCycle <= 255)
        {
          dutyCycle+=20;
          ledcWrite(ledChannel,dutyCycle); 
        }
        else{
          dutyCycle = 10;
          ledcWrite(ledChannel,dutyCycle);
        }        
      }      
    }    
  }  
 
  if (oneSecondFlag == 1)
  {
    /* code */
      oneSecondFlag = 0; 
      x++;
      led1stat=1-led1stat;
      digitalWrite(LED1, led1stat);   // turn the LED on or off
  }
  

}