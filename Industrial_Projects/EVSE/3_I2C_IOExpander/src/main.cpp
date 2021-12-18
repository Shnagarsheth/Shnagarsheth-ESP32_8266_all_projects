#include <Arduino.h>
 //#include <MCP2301x.h>

// #include "MCP23018.h"
#include<Wire.h>

// MCP23018 IOExp;
// const uint8_t I2C_MCP23018 = B0100000;
const uint8_t I2C_MCP23018 = B0100000;
uint8_t toggle = 0X55;

// MCP23018 registers
const uint8_t IODIRA = 0x0;
const uint8_t IODIRB = 0x1;
const uint8_t IOCON = 0xA;
const uint8_t IOCON_SEQOP[] = { IOCON, 5 };
const uint8_t GPPUA = 0x0C;
const uint8_t GPPUB = 0x0D;
const uint8_t GPIOA = 0x12;
const uint8_t GPIOB = 0x13;
const uint8_t OLATA = 0x14;
const uint8_t OLATB = 0x15;

uint8_t received_data;
char c;
#define Output 0XFF

void setup() {
  Wire.begin();
  Wire.setClock(100000);
  Wire.beginTransmission(I2C_MCP23018);
  Wire.write(IODIRB);
  Wire.write(0xFF);
  Wire.endTransmission();

  Wire.beginTransmission(I2C_MCP23018);
  Wire.write(IODIRA);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.beginTransmission(I2C_MCP23018);
  Wire.write(GPPUA);
  Wire.write(0xff);
  Wire.endTransmission();

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  //byte errorCode;
  
  toggle = ~toggle;

  Wire.beginTransmission(I2C_MCP23018);
  Wire.write(GPIOA);
  Wire.write(toggle);
  Wire.endTransmission();
  
  Wire.beginTransmission(I2C_MCP23018);
  Wire.write(GPIOB);
  Wire.endTransmission();
 
  Wire.requestFrom(I2C_MCP23018,(uint8_t)1);
  while(Wire.available()){
    c = Wire.read();
  }
  Serial.printf("IN: %X  OUT: %x\n",c,toggle);
  delay(1000);

  // Wire.beginTransmission(0xA0);
  // Wire.write(0x00);
  // Wire.write(12);
  // Wire.endTransmission();

  // Wire.requestFrom(0xA0,(uint8_t)1);
  // while(Wire.available()){
  //   Serial.println(Wire.read());
  // }

  delay(1000);

}