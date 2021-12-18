
// #include <Wire.h>;
// #include <inttypes.h>;

#define NUM_MCP2301x_ICs   1

#include <MCP2301x.h>

#ifdef _DEBUG_

void errorFunction(String errMessg) {
    Serial.println(errMessg);
}

MCP2301x IOexp(errorFunction);

#else

MCP2301x IOexp;

#endif // _DEBUG_

#define IOEXP_MODE  (IOCON_INTCC | IOCON_INTPOL | IOCON_ODR | IOCON_MIRROR)
#define ADDRESS      (MCP2301X_CTRL_ID+7)
#define LED_PIN      D4   // WeMOS D1 & D1 mini
#define IN_PORT      0
#define OUT_PORT     1

#define RELAY_ON      LOW
#define RELAY_OFF     HIGH
#define ALL_OFF       ALL_HIGH

void setup()
{
  Serial.begin(9600);
  while( ! Serial) delay(1);
  Serial.println("\n\nIOexpander example");

  
  if(IOexp.init(ADDRESS, IOEXP_MODE, I2C_FREQ100K)) { 
    Serial.println("Communication with IOexpander works!");
  } else {
    Serial.print("No communication with the IOexpander!!");
  }    

  pinMode(LED_PIN,OUTPUT);
  
  IOexp.pinPolarityPort(OUT_PORT, ALL_INVERTED);
  IOexp.pinModePort(OUT_PORT,ALL_OUTPUT); 
  IOexp.pinModePort(IN_PORT,ALL_INPUT);
}

int sw = 0;
int x = 500;
long next = 1000;
long next2 = 1000;

void loop()
{
  static bool on = 0;
  //blinks port OUT_PORT.7 to OUT_PORT.4
  
  if(millis() >= next){
    if(on) {
        IOexp.digitalWritePort(OUT_PORT, ALL_OFF);
        on = 0;
    } else {
        switch(sw){
          case 0:
            IOexp.digitalWrite(OUT_PORT,4,RELAY_ON);
            sw++;
            break;
          case 1:
            IOexp.digitalWrite(OUT_PORT,5,RELAY_ON);
            sw++;
            break;
          case 2:
            IOexp.digitalWrite(OUT_PORT,6,RELAY_ON);
            sw++;
          break;
          case 3:
            IOexp.digitalWrite(OUT_PORT,7,RELAY_ON);
            sw= 0;
            Serial.print("next = "); Serial.println(next,DEC); 
            on = 1;            
          break; 
          default:
            sw= 0;
            on = 1;            
            break;
        }
    }
    next = millis() + x;
  }
/*  
  //check the buttons on the IO expander
  if(millis() >= next2){
    if(IOexp.digitalRead(IN_PORT,0)){
      digitalWrite(LED_PIN,HIGH);
      x = 100;
    }else {
      x = 500;
      digitalWrite(LED_PIN,LOW);
    }
    next2 = millis() + 50;
  } */
}
