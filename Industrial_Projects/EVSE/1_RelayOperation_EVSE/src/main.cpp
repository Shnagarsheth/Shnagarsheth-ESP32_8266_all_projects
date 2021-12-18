/**
 * Written by: Shaival Nagarsheth
 * For: National Infotech
 * */
#include <Arduino.h>

hw_timer_t * timer1 = NULL;
portMUX_TYPE timerMux1 = portMUX_INITIALIZER_UNLOCKED;

#define RLY2 27
#define RLY3 4

volatile uint8_t relay2state = 0, relay3state = 0, timerCounter = 0, flagKeyScan = 0, twoSecondFlag =0, buttonFlag = 0;

void IRAM_ATTR onTimer1(){
  // Critical Code here
  portENTER_CRITICAL_ISR(&timerMux1);
  timerCounter+=1;
  if(timerCounter == 80000){
    flagKeyScan=1;
  }
  if (timerCounter == 200000)
  {
    /* code */
    timerCounter = 0;
    twoSecondFlag = 1;
  }
    portEXIT_CRITICAL_ISR(&timerMux1);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(RLY2, OUTPUT);
  pinMode(RLY3, OUTPUT);
  digitalWrite(RLY2, LOW);
  digitalWrite(RLY3, LOW);
  delay(10);

  Serial.println("Start timer 1");
  timer1 = timerBegin(1, 80, true);  // timer 1, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 80 -> 1000 ns = 1 us, countUp
  timerAttachInterrupt(timer1, &onTimer1, true); // edge (not level) triggered 
  timerAlarmWrite(timer1, 10, true); // 10 * 1 us = 10 us, autoreload true

  // Enable the timer alarms
   timerAlarmEnable(timer1); // enable
}

void loop() {
  // put your main code here, to run repeatedly:

      Serial.println("ON: Relay 2");      
      digitalWrite(RLY2, HIGH);   // Trigger RELAY2
      delay(2000);

      Serial.println("ON: Relay 3");
      digitalWrite(RLY3, HIGH);  // Trigger RELAY3
      delay(2000);

      Serial.println("OFF: Relay 2");
      digitalWrite(RLY2, LOW);   // Trigger RELAY2
      delay(2000);

      Serial.println("OFF: Relay 3");
      digitalWrite(RLY3, LOW);  // Trigger RELAY3
      delay(2000);
      
  // if (twoSecondFlag == 1){
  //   /* code */
  //     twoSecondFlag = 0; 
      
  //     relay2state=1-relay2state;
  //     relay3state=1-relay3state;
  //     Serial.println("Switching Relay 2");      
  //     digitalWrite(RLY2, relay2state);   // Trigger RELAY2
  //     Serial.println("Switching Relay 3");
  //     digitalWrite(RLY3, relay3state);  // Trigger RELAY3
  // }
}