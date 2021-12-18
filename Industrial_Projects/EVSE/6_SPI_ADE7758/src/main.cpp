#include <Arduino.h>

#define DIV 2
#define DAC1 25
//#define WATT_CAL 20.1

#define WATT_CAL 0.205638806

#define VAR_CAL 20.2
//#define VA_CAL 20.3

#define VA_CAL 0.2357520278

//#define VRMS_CAL 138.2382
//#define VRMS_CAL 132998.8316
// #define VRMS_CAL 4932.735426
// #define VRMS_CAL 4860.8042421564
// #define VRMS_CAL 4885.0574712643
#define VRMS_CAL 4700

//#define IRMS_CAL 20.6
//#define IRMS_CAL 0.0000057817
// #define IRMS_CAL 0.0057817
#define IRMS_CAL 0.005995

#define FREQ_CAL 20.67
#define Peak_Voltage 125
#define Peak_Current 0.5

// #include <avr/pgmspace.h>
// #include <avr/power.h>
#include "SPI.h"
#include "ADE7758.h"

unsigned char defaultValue;
unsigned char mmodeReg = 0x14;
float voltagePhaseB, currentPhaseB;
unsigned char b2=0x12, b1=0x34, b0=0x56; 
unsigned long re;
long cWaveDatePhaseB;
uint8_t dacvalue1;

  float Watt_a, Watt_b, Watt_c, wValue;
  float Var_a, Var_b, Var_c;
  float VA_a, VA_b, VA_c;
  float Tension_a, Tension_b, Tension_c;
  float Corriente_a, Corriente_b, Corriente_c;
  float C1_a, C2_b, C3_c;
  float Frequency, Freq1;

ADE7758 meter;
//SPIClass SPI1(HSPI);

void setup() {

  Serial.begin(115200);
//clock_prescale_set(16);
  Serial.println("ADE7758 test. Configurando..");
  meter.Init();
//  meter.setSPI();  // Setea el SPI para comunicarse con el ADE7758
  meter.setLcycMode(0); // Todos los bits en 0, ver el cpp para mas informacion.
  meter.gainSetup(INTEGRATOR_OFF, FULLSCALESELECT_0_25V, GAIN_1, GAIN_1);
  meter.setupDivs(DIV, DIV, DIV); //Divisor de los valores que se guardan en los registros de potencia
  meter.setPVoltageDetect(Peak_Voltage);
  // meter.setPCurrentDetect(Peak_Current);
//   meter.resetStatus();
// //  meter.setMaskInterrupts(0xFFFFF);
//   meter.setMaskInterrupts(0x01400);
  // meter.setWavMode(0x01);
  // meter.setMaskInterrupts(WFSM);    
//  meter.closeSPI();  // Deja el SPI en el estado original SPI_MODE0

  // Serial.println("..Configurado.");
}

/*****************************
		Main Loop
******************************/
void loop() {
//  re = (unsigned long)b2<<16 | (unsigned long)b1<<8 | (unsigned long)b0;
//  Serial.println(re);
//meter.setLcycMode(0xB7);
  //defaultValue = meter.getMMode();

  // digitalWrite(5, HIGH);
  // digitalWrite(5,LOW);
  // Serial.println("Default Values: ");
  // Serial.print(meter.getMMode(), HEX); Serial.print("---"); Serial.print(meter.getOpMode(), HEX); Serial.print("---"); 
  // Serial.print(meter.getWavMode()); Serial.print("---"); Serial.print(meter.getCompMode()); Serial.print("---"); Serial.println(meter.getLcycMode());
  // Serial.println("-------------");Serial.print(meter.getWavMode());Serial.print("---"); 
 
  ////======================COMIENZO DE MEDICIONES===============================
  //Serial.println("Measuring..");
//  meter.setSPI();

// Reading waveeform data for Phase B --
// meter.setWavMode(0x61);
// meter.setMaskInterrupts(WFSM);
// cWaveDatePhaseB = (meter.getWFORM() & 0x00FFFF);
// cWaveDatePhaseB = (meter.getWFORM() + 2642411);
// cWaveDatePhaseB = (meter.getWFORM());
// Serial.println(cWaveDatePhaseB, HEX);
// Serial.println(cWaveDatePhaseB, DEC);

// if((cWaveDatePhaseB & 0x800000) == 0x800000){
//   // cWaveDatePhaseB = ((~(cWaveDatePhaseB) + 0x000001) & 0x00FFFFFF);
//   cWaveDatePhaseB +=  0xFF000000;
// }
// // Serial.println(cWaveDatePhaseB, DEC);
// cWaveDatePhaseB +=2642412;
// wValue=(((float)cWaveDatePhaseB/(2*2642412.0f))*256.0f);

// // Serial.println(wValue, DEC);
// dacvalue1 = wValue;
// // Serial.println(dacvalue1, DEC);
// dacWrite(DAC1, dacvalue1);
// // dacWrite(DAC1, 0xFF);
// // dacWrite(DAC1, 0x00);
// digitalWrite(DAC1, HIGH);
// digitalWrite(DAC1, LOW);

// dacWrite(DAC1, 128);
// cWaveDatePhaseB &= 0X00FFFF;
//delay(200);
// Serial.print(meter.getWavMode());Serial.println("----------");
// Serial.print("WFORM value HEX: "); Serial.println(cWaveDatePhaseB, HEX);

// Serial.print("WFORM value DEC: "); Serial.println(cWaveDatePhaseB, DEC); 

Serial.println("--------------------------------");
//  Tension_a = meter.avrms();
//  Tension_b = meter.bvrms();
//  Tension_c = meter.cvrms();

    // Tension_a = meter.getAVRMS();
    Tension_b = meter.getBVRMS();
    // Tension_c = meter.getCVRMS();

//  Corriente_a = meter.airms();
    // Corriente_b = (meter.getBIRMS()/ 1000);
    
//  Corriente_c = meter.cirms();
  //
//   Serial.print("Version:");
//   Serial.println(meter.getVersion());
  
  // Frequency = meter.getFreq();
  // Serial.print("Freqency: "); Serial.println(Frequency);
  
  // long taim = millis();
  // if (meter.setPotLine(PHASE_A, 20)) {
  //   Watt_a = meter.getWatt(PHASE_A);
  //   Var_a = meter.getVar(PHASE_A);
  //   VA_a = meter.getVa(PHASE_A);
  // } else {
  //   Serial.println("There is no zero crossing in Phase A");
  // }
  
// //  Serial.print("Duro:");
// //  Serial.println((millis() - taim) / 1000);
  // taim = millis();
  if (meter.setPotLine(PHASE_B, 20)) {
    Watt_b = meter.getWatt(PHASE_B);
    Var_b = meter.getVar(PHASE_B);
    VA_b = meter.getVa(PHASE_B);
  } else {
    Serial.println("There is no zero crossing in Phase B");
  }
  
// //  Serial.print("Duro:");
// //  Serial.println((millis() - taim) / 1000);

//   taim = millis();
//   if (meter.setPotLine(PHASE_C, 20)) {
//     Watt_c = meter.getWatt(PHASE_C);
//     Var_c = meter.getVar(PHASE_C);
//     VA_c = meter.getVa(PHASE_C);
//   } else {
//     Serial.println("There is no zero crossing in Phase C");
//   }
  
//   //Serial.print("Duro:");
//  // Serial.println((millis() - taim) / 1000);
//   //meter.closeSPI();
//  // Serial.println("..Finalizo medicion");
//  // Serial.print("Memoria: "); Serial.println(freeRam());
//   //
  Serial.println("--------------Raw DATA:-------------");    
  // Serial.println("Phase A:");
// Serial.print("Freqency_A: "); Serial.println(Frequency_a);
  // Serial.print("VU:");  Serial.println( Tension_a, DEC );
  // Serial.print("I:");  Serial.println( Corriente_a,DEC );
// //    Serial.print("Watt:");  Serial.println( Watt_a,DEC );
// //    Serial.print("Var:");  Serial.println( Var_a,DEC );
// //    Serial.print("VA:");  Serial.println( VA_a,DEC );

//  Serial.println("---------------------------");  
  Serial.println("Phase B:");
// // // //  Serial.print("Freqency_B: "); Serial.println(Frequency_b);
  Serial.print("V:");  Serial.println( Tension_b, DEC );
  // Serial.print("I:");  Serial.println( Corriente_b,DEC );
//   Serial.println(C2_b);  
//     Serial.print("Watt:");  Serial.println( Watt_b,DEC );
//     Serial.print("Var:");  Serial.println( Var_b,DEC );
//     Serial.print("VA:");  Serial.println( VA_b,DEC );

// Serial.println("---------------------------");  
//   Serial.println("Phase C:");
// //  Serial.print("Freqency_C: "); Serial.println(Frequency_c);
//   Serial.print("V:");  Serial.println( Tension_c, DEC );
// //    Serial.print("I:");  Serial.println( Corriente_c,DEC );
// //    Serial.print("Watt:");  Serial.println( Watt_c,DEC );
// //    Serial.print("Var:");  Serial.println( Var_c,DEC );
// //    Serial.print("VA:");  Serial.println( VA_c,DEC );  //

  Serial.println("--------------------------------------------------------");
  
  Serial.println("-->Corrected values:---------------");
// //  Serial.print("Frequency: "); Serial.println(Frequency_a / FREQ_CAL);
  // Serial.println("Phase A:");
  // Serial.print("V:");  Serial.println( Tension_a / VRMS_CAL, DEC );
// //    Serial.print("I:");  Serial.println( Corriente_a/IRMS_CAL,DEC );
// //    Serial.print("Watt:");  Serial.println( Watt_a/WATT_CAL,DEC );
// //    Serial.print("Var:");  Serial.println( Var_a/VAR_CAL,DEC );
// //    Serial.print("VA:");  Serial.println( VA_a/VA_CAL,DEC );
// Serial.println("--------------------------------------------------------");
  Serial.println("Phase B:");
  Serial.print("V:");  Serial.println( Tension_b / VRMS_CAL, DEC );
  // Serial.print("I:");  Serial.println( ((Corriente_b*IRMS_CAL)/2.045), DEC);
  // voltagePhaseB = Tension_b / VRMS_CAL;
  // currentPhaseB = Corriente_b*IRMS_CAL;
  // Serial.print("I:");  Serial.println( ((Corriente_b*IRMS_CAL)), DEC);
// //    Serial.print("Watt:");  Serial.println( Watt_b*WATT_CAL,DEC );
// //    Serial.print("Var:");  Serial.println( Var_b/VAR_CAL,DEC );
//     Serial.print("VA:");  Serial.println( (VA_b * VA_CAL),DEC );
// Serial.println("--------------------------------------------------------");
//   Serial.println("Phase C:");
//   Serial.print("V:");  Serial.println( Tension_c / VRMS_CAL, DEC );
// //    Serial.print("I:");  Serial.println( Corriente_c/IRMS_CAL,DEC );
// //    Serial.print("Watt:");  Serial.println( Watt_c/WATT_CAL,DEC );
// //    Serial.print("Var:");  Serial.println( Var_c/VAR_CAL,DEC );
// //    Serial.print("VA:");  Serial.println( VA_c/VA_CAL,DEC );
// Serial.println("--------------------------------------------------------");
  delay(2000);
} // -- END of main loop

// Determines how much RAM is currently unused
// int freeRam () {
//   extern int __heap_start, *__brkval;
//   int v;
//   return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
// }