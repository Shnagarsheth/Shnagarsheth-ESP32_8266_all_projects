#include<WiFi.h>
#include<WiFiClient.h>
#include<DNSServer.h>
#include<WebServer.h>

#include "index.h"

#define portpin 34 //ADC1 Channel6
#define LED 2 //On Board LED

// Replace with your network credentials
const char* ssid = "TP-LINK_39FA";
const char* password = "Qwertyuiop";

WebServer server(80);

void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}

void handleADC() {
 int a = analogRead(portpin);
 String adcValue = String(a);
 
 server.send(200, "text/plane", adcValue); //Send ADC value only to client ajax request
}

void handleLED() {
 String ledState = "OFF";
 String t_state = server.arg("LEDstate"); //Refer  xhttp.open("GET", "setLED?LEDstate="+led, true);
 Serial.println(t_state);
 if(t_state == "1")
 {
  digitalWrite(LED,LOW); //LED ON
  ledState = "ON"; //Feedback parameter
 }
 else
 {
  digitalWrite(LED,HIGH); //LED OFF
  ledState = "OFF"; //Feedback parameter  
 }
 
 server.send(200, "text/plane", ledState); //Send web page
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED,OUTPUT); 
  
//===================================================================================================

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  // if you get here you have connected to the WiFi
  Serial.println("Connected.");

//=================================================================================================

  server.on("/",handleRoot);
  server.on("/setLED", handleLED);
  server.on("/readADC",handleADC);
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
}
