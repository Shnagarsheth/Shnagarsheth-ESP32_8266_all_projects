#include <Arduino.h>
#define EN485 4

volatile uint8_t total_buffer_size =21, modBusData_8Bit[50], data_ready=0,k=0, lenModData=0;
volatile uint16_t modBusData_16Bit[10];
String InString="";

// void getModBusData(){
// if(Serial.available()){
//   int rlen = Serial.readBytes(modBusData_8Bit, 21);
//   Serial.println(rlen);
//   for (int i=0;i<rlen;i++){
//     Serial.println(modBusData_8Bit[i]+0x00);
//     delayMicroseconds(100);
//   }
// }

void regenModBusData(){
  unsigned int i,j=0; 
  for(i=3;i<total_buffer_size;i+=2){    
    modBusData_16Bit[j]=((((uint16_t)modBusData_8Bit[i]<<8)&0xFF00) + ((uint16_t)modBusData_8Bit[i+1]&0x00FF));
    j++;
  }
 data_ready=true;
 total_buffer_size=0;
 k=0;
 lenModData=j;
}

void getuartdata(){
  while(Serial.available()){
    modBusData_8Bit[k]=Serial.read();
    k++; 
    total_buffer_size=k;  
  }  
  regenModBusData();
}

void setup() {
  Serial.begin(9600);
  pinMode(EN485, OUTPUT);
  digitalWrite(EN485, LOW);  
}

void loop() {
  // put your main code here, to run repeatedly:
  
  digitalWrite(EN485, HIGH);
  Serial.write(0x01);
  Serial.write(0x04);
  Serial.write(0x00);
  Serial.write(0x00);
  Serial.write(0x00);
  Serial.write(0x08);
  Serial.write(0xF1);
  Serial.write(0xCC);
  Serial.flush();
  delayMicroseconds(120);
  digitalWrite(EN485, LOW);
  getuartdata();  
  delay(2000);
//   if(data_ready){
//     data_ready = 0; 
//     uint8_t l=0, e=1;
//     for(l=1;l<lenModData;l+=2){    
//       Serial.print("Temp Channel ");  Serial.print(e); Serial.print(": "); Serial.println((float) modBusData_16Bit[l]/100);
//       e++;
//     }
//     l=0;
//   }
}  