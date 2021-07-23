
/********************
  Shaival Nagarsheth
  UART + ThingSpeak (STM32 to ESP32)
  ******************/

// Load Wi-Fi library and ThingSpeak library 
#include<WiFi.h>
#include "ThingSpeak.h"
#include "Arduino.h"
#include "esp_task_wdt.h"


//Define Timer
hw_timer_t * timer0 =NULL;
hw_timer_t * timer1 =NULL;
//portMUX_TYPE timerMux0=portMUX_INITIALIZER_UNLOCKED;
//portMUX_TYPE timerMux1=portMUX_INITIALIZER_UNLOCKED;

//String for SERIAL 2 receive
String InString="";

float Val[100];
int Total_var, ind[10];

bool stringcomplete=false;

//Define pins for UART2
#define RX2 16 
#define TX2 17

//Define Inbuilt LED pin
const int LED = 2;
volatile uint8_t ledstat=0, flag1=0, flag15=0;

//WiFi SSiD and Password
#define ssid "TP-LINK_39FA"
#define pass "Qwertyuiop"

// Your channel ID and Key
#define SH_Channel_ID 1300507
#define SH_API_Key  "FGFYURJFTB2XF270"

WiFiClient client;
int counter=0,k=0,j=0,c=0;
int status=WL_IDLE_STATUS;

//Timer Interrupt Routine
void IRAM_ATTR onTimer0()
  {
   // portENTER_CRITICAL_ISR(&timerMux0);
    serialEvent2(); 
   // portEXIT_CRITICAL_ISR(&timerMux0);  
  }

void IRAM_ATTR onTimer1()
  {
  //  portENTER_CRITICAL_ISR(&timerMux1);
      flag15=1;
  // esp_task_wdt_reset();
  // portEXIT_CRITICAL_ISR(&timerMux1);  
  }

void setup() {
  // put your setup code here, to run once:
   
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, RX2, TX2);
    delay(10);
    Serial.println("UART2 Txd is on pin: 17");
    Serial.println("UART2 Rxd is on pin: 16");
    Serial.println("UART2 Ready");
 // TIMER_SETUP_SH();
   
// ESP-32 TIMER SETTINGS
    timer1=timerBegin(1,80,true);
    timerAttachInterrupt(timer1, &onTimer1, true);
    timerAlarmWrite(timer1,15000000,true);
    timerAlarmEnable(timer1);
    
Serial.println("Start timer0");
    timer0=timerBegin(0,80,true);
    timerAttachInterrupt(timer0, &onTimer0, true);
    timerAlarmWrite(timer0,1000000,true);
    
    timerAlarmEnable(timer0);
  
    
 // Initialize the output variables as outputs
  pinMode(LED, OUTPUT);   
      digitalWrite(LED,LOW);
    WIFI_SETUP_SH();
    ThingSpeak.begin(client);
}

//void TIMER_SETUP_SH()
//  {  
//    
//  }

void WIFI_SETUP_SH()
  {
       // Connect to Wi-Fi network with SSID and password
        Serial.println("Connecting");
        WiFi.begin(ssid, pass);
        while (WiFi.status() != WL_CONNECTED) 
              {
                delay(500);
                Serial.print(".");
               }
        if(WiFi.status()!=WL_CONNECTED)
               {
                 Serial.println("Failed");
                }
        else
            {
             Serial.println("Connected");
            Serial.println(WiFi.localIP());
            }        
  }

void loop() {
  // put your main code here, to run repeatedly:
   // ESP.wdtFeed();       //resetting the watchdog as it causes restarting of the controller
   // esp_task_wdt_reset();
     ThingSpeak.setField(1,counter);
    ThingSpeak.setField(2,Val[0]);
    ThingSpeak.setField(3,Val[1]);
    ThingSpeak.setField(4,Val[2]);

if(flag15)
  {
    flag15=0;
    ledstat=1-ledstat;
    digitalWrite(LED,ledstat);
    counter++;    
    ThingSpeak.writeFields(SH_Channel_ID,SH_API_Key);
  }  

if(stringcomplete)
  {
    Serial.println(InString);
    Serial.print("Stringlength:");
    Serial.println(InString.length());

for(int x=0;x<=InString.length();x++)
{
  if(InString.substring(x,x+1)==",")
    {
      c++;  
    }
   else if(InString.substring(x,x+1)==".")
   {
    Total_var=c+1;
    }
}   
   // Total_var=c+1;
  for(int f=0;f<Total_var;f++){
    ind[k]=InString.indexOf(',',j);
    Val[k]=InString.substring(j,ind[k]).toFloat();
    Serial.println(Val[k]);
    j=ind[k]+1;
    k++;
    if(f==(Total_var-1)){
    Val[k]=InString.substring(j).toFloat();
    }
    } 
    InString="";
    //Val[100]=0;
    ind[10]=0;
    j=0;k=0;c=0;
    stringcomplete=false;
  }
//ThingSpeak.writeFields(SH_Channel_ID,SH_API_Key);
//delay(10);

//  counter++;
//  AD1 =AD1+1.23;
//  Serial.println(AD1);
//  ThingSpeak.setField(1,Val[]);
//  ThingSpeak.setField(2,AD1);
//  ThingSpeak.setField(3,WiFi.RSSI());
//  
//  ThingSpeak.writeFields(SH_Channel_ID,SH_API_Key);
// // ThingSpeak.writeField(SH_Channel_ID,1,counter,SH_API_Key);
//  delay(15000);
}

void serialEvent2()
{
  while(Serial2.available())
  {
    char InChar=(char)Serial2.read();
    if(isWhitespace(InChar))
      {
      InString+=',';
      }
    else if(InChar=='\n')
            { 
             InString+='.';
             stringcomplete=true;    
             }
    else{
          InString+=InChar;
        }            
   } 
}
