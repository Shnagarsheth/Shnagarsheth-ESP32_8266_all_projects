hw_timer_t * timer =NULL;
//portMUX_TYPE timerMux=portMUX_INITIALIZER_UNLOCKED;
int LED1 = 2;
volatile uint8_t LED_status =0;
String InString="";
bool stringcomplete=false;
#define RX2 16 
#define TX2 17
String header;
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
 
#define LED1 2 

//decalaration for UART2 DATA
char data[255];
#define RX2 16 
#define TX2 17

// Decalaration for Wifi
const char *ssid = "MyESP32";
const char *password = "Pass";
int w=0;
int d=1;
  IPAddress ip(1,2,3,4);
  IPAddress gateway(1,2,3,1);
  IPAddress subnet(255,255,255,0);

WiFiServer server(80);

void IRAM_ATTR onTimer()
  {
    serialEvent2();
 
  }

void setup() {
  // put your setup code here, to run once:
  pinMode(LED1, OUTPUT);
 
  Serial.begin(115200);
 // Serial.begin(57600);
  
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2);

// ESP-32 TIMER SETTINGS
  timer=timerBegin(0,80,true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer,1000000,true);
  timerAlarmEnable(timer);
  
  Serial.println();
  Serial.println("Configuring access point...");
 
  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid);
  delay(100);
  Serial.println("Set softAPConfig");
 // IPAddress aIp(19, 16, 5, 1);
  WiFi.softAPConfig(ip, gateway,subnet);
   delay(100);
 IPAddress myIP = WiFi.softAPIP();
  
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();
 
  Serial.println("Server started...");
}
void loop() {
  // put your main code here, to run repeatedly:
  
  WiFiClient client = server.available();   // listen for incoming clients
 
  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c); 
        header +=c;
        // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character
 
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
 
            // the content of the HTTP response follows the header:
            client.println("<!DOCTYPE html><html>");
            client.print("<meta http-equiv=\"refresh\" content=\"d\">");
            client.print("<h1 style;>National Infotech</h1>");
            client.print("<h2>UART Web Server</h2><br>");
            client.print("Click <a href=\"/H\">here</a> to turn ON the LED.<br>");
          //  client.print("Click <a href=\"/K\">here</a> to icrement counter<br>");
            client.print("Click <a href=\"/L\">here</a> to turn OFF the LED.<br>");
           // client.print("Click <a href=\"/J\">here</a> to turn reset the counter<br>");
           // client.println(w + "<br>");
           
             if(stringcomplete)
                 {
                      Serial.println(InString);
                      client.println("<p>" + InString + "<br></p>"); 
                       InString="";
                       stringcomplete=false;                           
                   }                           
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
 
        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED1, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /K")) {
          w++;            
          }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED1, LOW);                // GET /L turns the LED off
        }
        if (currentLine.endsWith("GET /J")) {
          w=0;
        }
      }
    }
    
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }  
}


void serialEvent2()
{
  //char InChar;
 // int i;
  while(Serial2.available())
  {
  char InChar=(char)Serial2.read();
   
//   if(InChar != '\n')
//      {
//      char InChar=(char)Serial2.read();
//      data[i]=InChar;
//      InString+=InChar;
//      i++;
//      }
    InString+=InChar;
    if(InChar=='\n')
    {
      stringcomplete=true;
    } 
  }
}
