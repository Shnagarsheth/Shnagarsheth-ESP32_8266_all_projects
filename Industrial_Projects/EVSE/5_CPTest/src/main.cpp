// #include <Arduino.h>
// #include "CP.h"

// #define PWM1  32 //PWM pin 
// #define CPIN1 34 //ADC2 pin
// #define DAC1  26  //VBAT pin temporary for DAC..
// #define C16A  17477
// #define C32A  34931
// #define C15A  16384
// #define C30A  32768
// #define DC100 65536

// float CPValue=0, DACValue=0, pValuePWM=0, pilotHV=0;
// uint16_t avg[50]; uint8_t i=0;  uint16_t maxValue=0;

// // setting PWM properties
// const int freq = 1000; //1KHz signal
// const int ledChannel = 0; //Need to choose a PWM channel. There are 16 channels from 0 to 15.
// const int resolution = 16; // Need to set the signal’s duty cycle resolution: you have resolutions from 1 to 16 bits.
// //We’ll use 16-bit resolution, which means you can control the LED brightness using a value from 0 to 65536.
// int dutyCycle = DC100;

// void setup() {
//   // put your setup code here, to run once:
//   Serial.begin(115200);
//   //COnfiguring PWM functionalities
//   ledcSetup(ledChannel, freq, resolution);
//   //Attaching channel to the GPIO pin
//   ledcAttachPin(PWM1, ledChannel);
// }

// void loop() {
//   // put your main code here, to run repeatedly:
//    uint8_t k=0;
//    ledcWrite(ledChannel,dutyCycle);
//    CPValue = analogRead(CPIN1);
//    avg[i] = CPValue;
//    i++;
//   //  DACValue = (256*CPValue)/4096;
//   //  dacWrite(DAC1, DACValue);
//   if(i==50){
//     i=0;
//     maxValue=0;
//     for (k=0;k<50;k++){
//       if(avg[k]>maxValue){
//         maxValue=avg[k];
//       }
//     }
//     pilotHV=maxValue;  
//     if (pilotHV > 3200)
//   {
//     Serial.print("State A: Not Connected  "); Serial.println(pilotHV);
//   }
//   else if ((pilotHV < 3110) & (pilotHV > 2870)){
//     Serial.print("State B: EV Connected Ready to Charge  "); Serial.println(pilotHV);
//   }
//   else if ((pilotHV < 2735) & (pilotHV > 2430))
//   {
//     Serial.print("State C: EV charging   ");Serial.println(pilotHV);
//   }
//   else if ((pilotHV < 2300) & (pilotHV > 2005))
//   {
//     Serial.print("State D: EV charging Ventilation Required   "); Serial.println(pilotHV);
//   }
//   else if (pilotHV < 1990)
//   {
//     Serial.print("State E: Error   "); Serial.println(pilotHV);
//   }
//   else Serial.println(pilotHV);     
//   }
  
// }

#include <Arduino.h>
#include "CP.h"

char cPilotState=0;

CP cPilot;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  cPilot.CPInIt(C16A, 1);
}

void loop() {
  // put your main code here, to run repeatedly:
    cPilotState = cPilot.getCPEVState(1);
    Serial.println(cPilotState, DEC);
}