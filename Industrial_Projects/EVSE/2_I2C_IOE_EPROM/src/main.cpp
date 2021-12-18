#include <Arduino.h>
#include<Wire.h>

#define disk1 0x50   

//#define MCPAddress  0x20

void writeEEPROM(int deviceaddress, unsigned int eeaddress, byte data ) 
{
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress));
  Wire.write(data);
  Wire.endTransmission();
 
  delay(5);
}
 
byte readEEPROM(int deviceaddress, unsigned int eeaddress ) 
{
  byte rdata;
 
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress));
  Wire.endTransmission(); 
  Wire.requestFrom(deviceaddress,1);
 
  if (Wire.available()) rdata = Wire.read();
 
  return rdata;
}


void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(115200);
  unsigned int address = 0;
 
  writeEEPROM(disk1, address, 0x55);
  address=10;
  writeEEPROM(disk1, address, 0xAA);
  Serial.println(readEEPROM(disk1, address), HEX);
  address=0;
  Serial.println(readEEPROM(disk1, address), HEX);
}

void loop() {
  // put your main code here, to run repeatedly:
  byte error, address;
  int nDevices;
  // Serial.println("Scanning...");
  nDevices = 0;

  // for(address = 1; address < 127; address++ ) {
  //   Wire.beginTransmission(address);
  //   error = Wire.endTransmission();
  //   if (error == 0) {
  //     Serial.print("I2C device found at address 0x");
  //     if (address<16) {
  //       Serial.print("0");
  //     }
  //     Serial.println(address,HEX);
  //     nDevices++;
  //   }
  //   else if (error==4) {
  //     Serial.print("Unknow error at address 0x");
  //     if (address<16) {
  //       Serial.print("0");
  //     }
  //     Serial.println(address,HEX);
  //   }    
  // }
  // if (nDevices == 0) {
  //   Serial.println("No I2C devices found\n");
  // }
  // else {
  //   Serial.println("done\n");
  // }
  // delay(5000);            
}