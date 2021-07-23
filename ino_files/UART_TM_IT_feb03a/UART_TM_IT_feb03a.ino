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
    serialEvent2();
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
  timer=timerBegin(0,80,true); // timer 0, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 80 -> 1000 ns = 1 us, countUp
  timerAttachInterrupt(timer, &onTimer, true); // edge (not level) triggered 
  timerAlarmWrite(timer,1000000,true); // 1000000 * 1 us = 1 s, autoreload true
  timerAlarmEnable(timer); // enable

  //Serial2.begin(baudrate,protocol,RX2,TX2);
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2);
  delay(10);
  Serial.println("UART2 Txd is on pin: 17");
  Serial.println("UART2 Rxd is on pin: 16");
  Serial.println("UART2 Ready");
}

void loop() {
  // put your main code here, to run repeatedly:
  if(stringcomplete)
    {
      Serial.println(InString);
      InString="";
      stringcomplete=false;     
    }
}

void serialEvent2()
{
  while(Serial2.available())
  {
   // digitalWrite(LED1,HIGH);
    char InChar=(char)Serial2.read();
    InString+=InChar;
    if(InChar=='\n')
    {
      stringcomplete=true;
    }
//  if(stringcomplete)
//    {
//      Serial.println(InString);
//      InString="";
//      stringcomplete=false;     
//    }

  }
}
