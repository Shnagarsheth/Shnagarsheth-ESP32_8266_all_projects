
/********************
  Shaival Nagarsheth
  UART + ThingSpeak (STM32 to ESP32)
  ******************/

// Load Wi-Fi library and ThingSpeak library 
#include<WiFi.h>
#include "ThingSpeak.h"
#include "Arduino.h"


//Define Timer
hw_timer_t * timer =NULL;

//String for SERIAL 2 receive
String InString="",sub1="",sub2="",sub3="",trimmed="";

float Val[100];
int Total_var, ind[10];

bool stringcomplete=false;

//Define pins for UART2
#define RX2 16 
#define TX2 17

//WiFi SSiD and Password
#define ssid "TP-LINK_39FA"
#define pass "Qwertyuiop"

// Your channel ID and Key
#define SH_Channel_ID 1300507
#define SH_API_Key  "FGFYURJFTB2XF270"

//WiFiClient client;

int counter=0,k=0,j=0,c=0;
float AD1=0.0;
//int status=WL_IDLE_STATUS;

//Timer Interrupt Routine
void IRAM_ATTR onTimer()
  {
    serialEvent2(); 
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
       Serial.println("Start timer");
    // ESP-32 TIMER SETTINGS
       timer=timerBegin(0,80,true);
       timerAttachInterrupt(timer, &onTimer, true);
       timerAlarmWrite(timer,1000000,true);
       timerAlarmEnable(timer);
       
 //   WIFI_SETUP_SH();
  //  ThingSpeak.begin(client);

    //InString.reserve(200);sub1.reserve(200);sub2.reserve(200);sub3.reserve(200);
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
    Val[100]=0;
    ind[10]=0;
    j=0;k=0;c=0;
    stringcomplete=false;
  }

//delay(10);

//  counter++;
//  AD1 =AD1+1.23;
//  Serial.println(AD1);
//  ThingSpeak.setField(1,counter);
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
