#include <Arduino.h>
#include <WiFi.h>
#include "ThingSpeak.h"

#define ssid "TP-LINK_39FA"
#define pass "Qwertyuiop"
#define SH_Channel_ID 1300507
#define SH_API_Key  "FGFYURJFTB2XF270"

WiFiClient client;

int counter=0;
float AD1=0.0;
int status=WL_IDLE_STATUS;

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
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
   ThingSpeak.begin(client);
   delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
  counter++;
  AD1 =AD1+1.23;

  Serial.println(AD1);

  ThingSpeak.setField(1,counter);
  ThingSpeak.setField(2,AD1);
  ThingSpeak.setField(3,WiFi.RSSI());

/*To write all the fields at a time*/  
  ThingSpeak.writeFields(SH_Channel_ID,SH_API_Key);

/*The Below function is used to write a single field*/
// ThingSpeak.writeField(SH_Channel_ID,1,counter,SH_API_Key);  
  
  delay(15000);
}