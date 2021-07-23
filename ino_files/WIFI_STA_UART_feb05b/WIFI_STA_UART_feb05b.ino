/*********
  Shaival Nagarsheth
  UART Web Server 
*********/

// Load Wi-Fi library
#include <WiFi.h>


//Define Timer
hw_timer_t * timer =NULL;

//String for SERIAL 2 receive
String InString="";
bool stringcomplete=false;

//Define pins for UART2
#define RX2 16 
#define TX2 17

// Replace with your network credentials
const char* ssid = "TP-LINK_39FA";
const char* password = "Qwertyuiop";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String LEDstate = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int LED = 2;
const int output27 = 27;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;


//Timer Interrupt Routine
void IRAM_ATTR onTimer()
  {
    serialEvent2(); 
  }

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2);

  // ESP-32 TIMER SETTINGS
  timer=timerBegin(0,80,true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer,500000,true);
  timerAlarmEnable(timer);
  
  // Initialize the output variables as outputs
  pinMode(LED, OUTPUT);
  pinMode(output27, OUTPUT);
  
  // Set outputs to LOW
  digitalWrite(LED, LOW);
  digitalWrite(output27, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
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
  server.begin();
}

void loop(){
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
              digitalWrite(LED, HIGH);
            } else if (header.indexOf("GET /2/off") >= 0) {
              Serial.println("LED OFF");
              LEDstate = "OFF";
              digitalWrite(LED, LOW);
            } else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("Getting Error History");
              if(Serial2.availableForWrite())
                {
                  Serial2.write(85);
                }
              //output27State = "on";
             // digitalWrite(output27, HIGH);
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("GPIO 27 off");
              output27State = "off";
              //digitalWrite(output27, LOW);
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
            client.println("<body><h3>UART Web Server</h3>");
            
            // Display current state, and ON/OFF buttons for LED 
            client.println("<p>LED is " + LEDstate + " <br>Click to turn on/off</p>");
            // If the LED is off, it displays the ON button       
            if (LEDstate=="OFF") {
              client.println("<p><a href=\"/2/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/2/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Button for getting Error History...
            client.println("<p> Click the <b style=color: #CB4335;> Red </b> button to get <b> Error History </b></p>");
            // If the output27State is off, it displays the ON button       
            if (output27State=="off") {
              client.println("<p><a href=\"/27/on\"><button class=\"button button2\">Error History</button></a></p>");
            } else {
              client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
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

void serialEvent2()
{
  while(Serial2.available())
  {
    char InChar=(char)Serial2.read();
    InString+=InChar;
    if(InChar=='\n')
    {
      stringcomplete=true;
    } 
  }
}
