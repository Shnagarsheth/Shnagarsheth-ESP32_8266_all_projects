#include <Ticker.h>

//Initialize Timers
Ticker Timer1;
Ticker Timer2;

//Define onBoard LED pins
#define LED1 16
#define LED2 2

//Define status for LED pins
volatile uint8_t LED1_status, LED2_status =0;

//Interrupt Service Routine for Timer1
void ISR1(){
    LED1_status = 1- LED1_status;
    digitalWrite(LED1, LED1_status);    
  }
//Interrupt Service Routine for Timer2
void ISR2(){
    LED2_status = 1- LED2_status;
    digitalWrite(LED2, LED2_status);
  }  
  
void setup() {
  // put your setup code here, to run once:
  //Configure LEDs as Output
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  //Attach Interrupt Service routine to be called at particular time
  Timer1.attach(5, ISR1); //ISR1 will be called every 5 seconds. 
  Timer2.attach(2,ISR2);  //ISR2 will be called every 2 seconds. 
}

void loop() {
  // put your main code here, to run repeatedly:
}
