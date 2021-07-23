#include <Arduino.h>

hw_timer_t * timer0 = NULL;
hw_timer_t * timer1 = NULL;
hw_timer_t * timer2 = NULL;
portMUX_TYPE timerMux0 = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE timerMux1 = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE timerMux2 = portMUX_INITIALIZER_UNLOCKED;

#define LED1 2
#define LED2 5
#define LED3 18
#define PWM1 16

volatile uint8_t led1stat = 0; 
volatile uint8_t led2stat = 0; 
volatile uint8_t led3stat = 0;

// setting PWM properties
const int freq = 5000; //5KHz signal
const int ledChannel = 0; //Need to choose a PWM channel. There are 16 channels from 0 to 15.
const int resolution = 8; // Need to set the signal’s duty cycle resolution: you have resolutions from 1 to 16 bits.
//We’ll use 8-bit resolution, which means you can control the LED brightness using a value from 0 to 255.
int dutyCycle = 128;

void IRAM_ATTR onTimer0(){
  // Critical Code here
  portENTER_CRITICAL_ISR(&timerMux0);
  led1stat=1-led1stat;
  digitalWrite(LED1, led1stat);   // turn the LED on or off
  portEXIT_CRITICAL_ISR(&timerMux0);
}

void IRAM_ATTR onTimer1(){
  // Critical Code here
  portENTER_CRITICAL_ISR(&timerMux1);
  led2stat=1-led2stat;
  digitalWrite(LED2, led2stat);   // turn the LED on or off
  portEXIT_CRITICAL_ISR(&timerMux1);
}

void IRAM_ATTR onTimer2(){
  // Critical Code here
  portENTER_CRITICAL_ISR(&timerMux2);
  led3stat=1-led3stat;
  digitalWrite(LED3, led3stat);   // turn the LED on or off
  portEXIT_CRITICAL_ISR(&timerMux2);
}

void setup() {
  Serial.begin(115200);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  digitalWrite(LED1, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(LED2, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(LED3, LOW);    // turn the LED off by making the voltage LOW
  delay(10);

  Serial.println("Start timer 0");
  timer0 = timerBegin(0, 80, true);  // timer 0, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 80 -> 1000 ns = 1 us, countUp
  timerAttachInterrupt(timer0, &onTimer0, true); // edge (not level) triggered 
  timerAlarmWrite(timer0, 1000000, true); // 1000000 * 1 us = 1 s, autoreload true
  
  Serial.println("Start timer 1");
  timer1 = timerBegin(1, 80, true);  // timer 1, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 80 -> 1000 ns = 1 us, countUp
  timerAttachInterrupt(timer1, &onTimer1, true); // edge (not level) triggered 
  timerAlarmWrite(timer1, 10, true); // 10 * 1 us = 10 us, autoreload true

  Serial.println("Start timer 2");
  timer2 = timerBegin(2, 80, true);  // timer 2, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 80 -> 1000 ns = 1 us, countUp
  timerAttachInterrupt(timer2, &onTimer2, true); // edge (not level) triggered 
  timerAlarmWrite(timer2, 750000, true); // 750000 * 1 us = 750 ms, autoreload true

  // Enable the timer alarms
  timerAlarmEnable(timer0); // enable
  timerAlarmEnable(timer1); // enable
  timerAlarmEnable(timer2); // enable

  //COnfiguring PWM functionalities
  ledcSetup(ledChannel, freq, resolution);

  //Attaching channel to the GPIO pin
  ledcAttachPin(PWM1, ledChannel);

}

void loop() {
  ledcWrite(ledChannel,dutyCycle);
}