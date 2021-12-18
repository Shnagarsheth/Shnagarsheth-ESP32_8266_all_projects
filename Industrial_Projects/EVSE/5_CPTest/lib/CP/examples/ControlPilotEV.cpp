#include <Arduino.h>
#include "CP.h"

char cPilotState=0;

CP cPilot;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  cPilot.CPInIt(DC100,1);
}

void loop() {
  // put your main code here, to run repeatedly:
  cPilotState = cPilot.getCPEVState(1);
  Serial.println(cPilotState, DEC);
}