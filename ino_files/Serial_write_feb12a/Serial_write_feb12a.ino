String InString="";
bool stringcomplete=false;

hw_timer_t * timer0 = NULL; //for timer0
hw_timer_t * timer1 = NULL; //for timer1 

#define RX2 16 
#define TX2 17

int LED1=2;
volatile uint8_t TSflag15=0,c=0,Total_count=10;
volatile uint16_t tripdata_16Bit[106];
volatile uint8_t tripdata_8Bit[106];

void IRAM_ATTR onTimer0()    //own func intrupt RAM attribute
    {
     c++;
     //serialEvent2();
    }
void IRAM_ATTR onTimer1()    //own func intrupt RAM attribute event 
    {
     TSflag15=1;
    }

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2);
  Serial.println("UART2 Txd is on pin: 17");
  Serial.println("UART2 Rxd is on pin: 16");
  Serial.println("UART2 Ready");
  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, LOW);
  InString.reserve(200);

   Serial.println("start timer1 ");
  //Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
  timer1 = timerBegin(1, 80, true);  // timer 1, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 80 -> 1000 ns = 1 us, countUp
  timerAttachInterrupt(timer1, &onTimer1, true); // edge (not level) triggered  /*
  timerAlarmWrite(timer1, 13000000, true); // 13000000 * 1 us = 15 s, autoreload true
  timerAlarmEnable(timer1); // enable
  
  timer0 = timerBegin(0, 80, true);  // timer 0, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 80 -> 1000 ns = 1 us, countUp
  timerAttachInterrupt(timer0, &onTimer0, true); // edge (not level) triggered  /*
  timerAlarmWrite(timer0, 1000, true); // 1000000 * 1 us = 1 s, autoreload true
  //timerAlarmEnable(timer0); // enable
}

void loop() {
  // put your main code here, to run repeatedly:
  if(stringcomplete)
    {
      Serial.println(InString);
      InString="";
      stringcomplete=false;     
    }  
   if(TSflag15)
      {
        TSflag15=0;
        Serial2.write(0x55);     // write a byte with the value 45 => '-' character
        serialEvent2();
        digitalWrite(LED1, HIGH);
      }    
}

//To get data Byte by Byte within 10ms. whole can be received within 10ms.
void serialEvent2()  
{
  timerAlarmEnable(timer0); // enable
 while(Serial2.available()){
  if(c<Total_count){
      char InChar=(char)Serial2.read();
       Serial.println(InChar);     
       InString+=InChar+0x00;
      }
   else{
    stringcomplete = true;
    c=0;
    } 
  }
}
