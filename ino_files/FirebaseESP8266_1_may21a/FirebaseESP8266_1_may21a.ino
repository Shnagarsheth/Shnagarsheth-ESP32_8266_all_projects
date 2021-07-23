#include "FirebaseESP8266.h"  
#include <ESP8266WiFi.h>
#include <Ticker.h>

Ticker Timer1;

#define FIREBASE_HOST "testsh-14676-default-rtdb.firebaseio.com" //--> URL address of your Firebase Realtime Database. https://testsh-14676-default-rtdb.firebaseio.com/
#define FIREBASE_AUTH "Ku1qCzVY9DFF19pOSCTt2TOKOzjnFUwnxJhlSNiY" //--> Your firebase database secret code.

#define led 2  //--> Defining an On Board LED, used for indicators when the process of connecting to a wifi router

//----------------------------------------SSID and Password of your WiFi router.
#define WIFI_SSID "TP-LINK_39FA" //--> Your wifi name or SSID.
#define WIFI_PASSWORD "Qwertyuiop" //--> Your wifi password.
//----------------------------------------


FirebaseData firebaseData;
FirebaseData ledData;

FirebaseJson json;

int n=0, oneSecond=0;

void ISR1(){
  oneSecond=1;
  }


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(led,OUTPUT);
  digitalWrite(led,HIGH);
   Timer1.attach(1, ISR1);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  delay(100);
  Firebase.setString(ledData,"/LED/Value","0");

}

void loop() {
  // put your main code here, to run repeatedly:
  
  if (oneSecond==1){
    oneSecond=0;
    n=n+1;  
  if(Firebase.setInt(firebaseData,"/Integer/Value", n)){
    Serial.print("Integer Value:");
    Serial.println(n);
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
    }
   else{
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
      }
    }
  
  if (Firebase.getString(ledData, "/LED/Value")){
    Serial.println(ledData.stringData());
    if (ledData.stringData() == "1") {
    digitalWrite(led, LOW);
    }
  else if (ledData.stringData() == "0"){
    digitalWrite(led, HIGH);
    }
  }  
}
