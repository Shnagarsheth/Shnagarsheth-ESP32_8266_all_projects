hw_timer_t * timer =NULL;
portMUX_TYPE timerMux=portMUX_INITIALIZER_UNLOCKED;
int LED1 = 2;
volatile uint8_t LED_status =0;
char data[255];
String InString="";
bool stringcomplete=false;
#define RX2 16 
#define TX2 17

void IRAM_ATTR onTimer()
  {
//    portENTER_CRITICAL_ISR(&timerMux);
   // serialEvent2();
//    if (Serial2.available()) 
//      {
//        Serial2.readBytes(data,28);  
//        Serial.print(data);
//      }
//  if(stringcomplete)
//    {
//      Serial.println(InString);
//      InString="";
//      stringcomplete=false;     
//    }
    LED_status = 1- LED_status;
    digitalWrite(LED1, LED_status); //Turn ON and OFF the LED 
//    portEXIT_CRITICAL_ISR(&timerMUX);
  }

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, LOW);
  delay(10);
  Serial.println("Start timer");
  
  // ESP-32 TIMER SETTINGS
  timer=timerBegin(0,80,true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer,1000000,true);
  timerAlarmEnable(timer);

  //Serial2.begin(baudrate,protocol,RX2,TX2);
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2);
  delay(10);
  Serial.println("UART2 Txd is on pin: 17");
  Serial.println("UART2 Rxd is on pin: 16");
  Serial.println("UART2 Ready");
}

void loop() {
  // put your main code here, to run repeatedly:

  Serial2.write(0x55);
  delay(10);

  if(Serial2.available()){
  int rlen = Serial2.readBytes(data,200);  //get 200 bytes at a time and store in buffer 'data'
  Serial.println(rlen);
  for(int i=0;i<rlen;i++)
  Serial.println(data[i]+0x00);  
  delay(100);    
  }
  
}
//
//void serialEvent2()
//{
//  while(Serial2.available())
//  {
//   // digitalWrite(LED1,HIGH);
//    char InChar=(char)Serial2.read();
//    InString+=InChar;
//    if(InChar=='\n')
//    {
//      stringcomplete=true;
//    }
//  }
//}
