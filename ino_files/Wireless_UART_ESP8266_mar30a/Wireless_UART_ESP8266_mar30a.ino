/******
 * Written By: Shaival Nagarsheth
 * Decription: Wireless UART. 
 */
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <Ticker.h>
#include <WiFiClient.h>

//Declare onBoard LED
#define LED1 16
#define LED2 2

//Define status for LED pins
volatile uint8_t LED1_status =0;

//Declare a String to Store the UART data from STM32 MCU.
String InString="";
bool stringcomplete=false;

// Variable to store the HTTP request
String header;

SoftwareSerial vUART (7,8); //Declaring 7 - Rx and 8 - Tx for software serial

Ticker Timer1; //Declaring timer 1 for ISR

const char *ssid = "Wireless_UART";
const char *password = "Password";

  IPAddress ip(1,2,3,4);
  IPAddress gateway(1,2,3,1);
  IPAddress subnet(255,255,255,0);

  WiFiServer server(80);

  
  // Auxiliar variables to store the current output state
  String LEDstate = "off";

   // Current time
  unsigned long currentTime = millis();
  // Previous time
  unsigned long previousTime = 0; 
  // Define timeout time in milliseconds (example: 2000ms = 2s)
  const long timeoutTime = 2000;

void ISR1(){
    LED1_status = 1- LED1_status;
    digitalWrite(LED1, LED1_status);  
    getuartdata();  
  }

void getuartdata(){
    while(vUART.available())
    {
      char InChar=(char)vUART.read();
      InString+=InChar;
      if(InChar=='\n')
      {
        stringcomplete=true;
      } 
    }
  }

void setup() {
  // put your setup code here, to run once:
  
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  Serial.begin(115200); //Starting Hardware Serial  
  vUART.begin(115200); //Starting software Serial for Communicating with STM32 MCU.
  Timer1.attach(1, ISR1); //ISR1 will be called every 5 seconds. 

  WiFi.softAP(ssid);
  delay(100);
  Serial.println("Set softAPConfig");
  WiFi.softAPConfig(ip, gateway,subnet);
  delay(100);
  IPAddress myIP = WiFi.softAPIP();
  
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();
  Serial.println("Server started...");  
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /2/on") >= 0) {
              Serial.println("LED ON");
              LEDstate = "ON";
              digitalWrite(LED2, LOW);
            } else if (header.indexOf("GET /2/off") >= 0) {
              Serial.println("LED OFF");
              LEDstate = "OFF";
              digitalWrite(LED2, HIGH);
            } 
            
           
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta http-equiv=\"refresh\"  name=\"viewport\" content=\"0.5, width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
           // client.println(".h1 {color: navy;");
            //client.println(".h2 {color: navy;");
            client.println(".button { background-color: #2874A6; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #CB4335;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1 style=color:navy;>National Infotech</h1>");
            client.println("<body><h2 style=color:navy;>A Way to Power Electrconics and Embeddded System Solutions...</h2>");
            client.println("<body><h3>Wireless-UART Web Server</h3>");
            
            // Display current state, and ON/OFF buttons for LED 
            client.println("<p>LED is " + LEDstate + " <br>Click to turn on/off</p>");
            // If the LED is off, it displays the ON button       
            if (LEDstate=="OFF") {
              client.println("<p><a href=\"/2/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/2/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
              client.println("<p> <b>Here is your UART Data:</b><br></p>"); 
               if(stringcomplete)
                 {
                    client.println("<p><br>" + InString + "<br></p>"); 
                    InString="";
                    stringcomplete=false;                           
                   }
            
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
