#include <Arduino.h>

#define LED1 2
int delaytime1 = 1000;
void Blink_LED(int led, int delaytime){
  Serial.println("LED ON");
  digitalWrite(led,HIGH);
  delay(delaytime);

  Serial.println("LED OFF");
  digitalWrite(led,LOW);
  delay(delaytime);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i=0;i<3;i++){
  Blink_LED(LED1, 500);
  }

  for (int i = 0; i < 2; i++){
  Blink_LED(LED1, delaytime1);
  }
}