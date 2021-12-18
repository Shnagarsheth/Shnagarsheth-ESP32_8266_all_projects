#include <Arduino.h>
#include "SPI.h"
#include "ADE7758.h"

hw_timer_t * timer1 = NULL;
portMUX_TYPE timerMux1 = portMUX_INITIALIZER_UNLOCKED;
volatile uint8_t LED_status =0;
#define DIV 2
#define DAC1 25

#define WATT_CAL 0.205638806
#define VAR_CAL 20.2
#define VA_CAL 0.2357520278
#define VRMS_CAL 4660.0574712643
#define IRMS_CAL 0.005995
#define FREQ_CAL 20.67

unsigned char defaultValue;
unsigned long re;
uint16_t i=0;
long cWaveDatePhaseB, cDataB[400];
uint8_t dacvalue1, readFlag;
float wValue;
  
ADE7758 meter;

void IRAM_ATTR onTimer1(){
  // Critical Code here
  portENTER_CRITICAL_ISR(&timerMux1);
  readFlag=1;
  portEXIT_CRITICAL_ISR(&timerMux1);
}

void setup() {

  Serial.begin(115200);
  Serial.println("ADE7758 test. Configurando..");
  pinMode(26,OUTPUT);
  digitalWrite(26, LOW); //disabled by default
  pinMode(27,OUTPUT);
  digitalWrite(27, LOW); //disabled by default

//------------------Initializing TIMER 1------------------------//
  timer1 = timerBegin(1, 80, true);  // timer 1, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 80 -> 1000 ns = 1 us, countUp
  timerAttachInterrupt(timer1, &onTimer1, true); // edge (not level) triggered 
  // timerAlarmWrite(timer1, 10, true); // 10 * 1 us = 10 us, autoreload true
  timerAlarmWrite(timer1, 156, true); // 170 * 1 us = 170 us, autoreload true
  // Enable the timer alarms
   timerAlarmEnable(timer1); // enable

//------------------Initializing Energy Meter Board with SPI Comm.------------------------//
  meter.Init();
  meter.setLcycMode(0); // Todos los bits en 0, ver el cpp para mas informacion.
  meter.gainSetup(INTEGRATOR_OFF, FULLSCALESELECT_0_25V, GAIN_1, GAIN_1);
  meter.setupDivs(DIV, DIV, DIV); //Divisor de los valores que se guardan en los registros de potencia
  meter.resetStatus();
  meter.setWavMode(0x01);
  meter.setMaskInterrupts(WFSM);    
}

/*****************************
		Main Loop
******************************/
void loop() {
//------------------ Reading waveeform data for Phase B ---------------------//

if(readFlag && (i<400)){

  // if(readFlag){
  readFlag=0;
  LED_status = 1- LED_status;
  digitalWrite(27, LED_status); 
  // GPIO.out_w1ts = ((uint32_t)1 << 26);  
  cWaveDatePhaseB = (meter.getWFORM());
  // GPIO.out_w1tc = ((uint32_t)1 << 26);  
  if((cWaveDatePhaseB & 0x800000) == 0x800000){
     cWaveDatePhaseB +=  0xFF000000;
  }
  cDataB[i]=cWaveDatePhaseB;
  i++;
}
// GPIO.out_w1tc = ((uint32_t)1 << 26);  
	
// Serial.println(cWaveDatePhaseB, DEC);
// cWaveDatePhaseB +=2642412;
// wValue=(((float)cWaveDatePhaseB/(2*2642412.0f))*256.0f);

// cWaveDatePhaseB +=1321206;
// wValue=(((float)cWaveDatePhaseB/(2*1321206.0f))*256.0f);

// dacvalue1 = wValue;
// dacWrite(DAC1, dacvalue1);

} // -- END of main loop