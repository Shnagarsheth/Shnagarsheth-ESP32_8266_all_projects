/*****
 * Written By:  Shaival Nagarsheth
 * Description: This sketch provides the UART Data from the stabilizer (Trip History/Trip Events). 
 *              On a button press in a web page, the trip history is updated. On button press 0x55 is sent (through UART) to 
 *              stabilizer controller (i.e. STM32G030), which in turn provides the trip data on uart.
 *              Blink LED is also embedded. 
 *              We first convert the 8bit received data into 16bit data.
 *              Moreover, Iin has to coverted in float value and devide by 100 in order to display it corrrectly.                 
 * *****/

#include <Arduino.h>
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <WebServer.h>
#else
  #error Invalid platform
#endif
#include <DNSServer.h>
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output5State = "off";

// Assign output variables to GPIO pins
const int LED = 2;
//const int output4 = 4;

//Define Timer
hw_timer_t * timer =NULL;

//String for SERIAL 2 receive
String InString="";
bool stringcomplete=false,data_ready=false;

//Define pins for UART2
#define RX2 16 
#define TX2 17

volatile uint8_t c=0,Total_count=10,k=0,tripdata_8Bit[106], total_buffer_size;
volatile uint16_t tripdata_16Bit[106];
volatile float I[5]; //to hold 5 float values of Current (I).

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void IRAM_ATTR onTimer()    //Timer 1 Interrupt Routine.. RAM attribute
    {
     c++; // this corresponds to increment of 1ms. 
    }


/* executes when data is available on serial port  */
void serialEvent2()
{
 timerAlarmEnable(timer); // TIMER ENABLE (Start the Timer)
 while(Serial2.available()){
  if(c<Total_count){
   tripdata_8Bit[k]=Serial2.read();        
    //Serial.println(tripdata_8Bit[k]);
     k++; 
     total_buffer_size=k;  
      }
   else{
    stringcomplete = true;
    c=0;    
    k=0;
    } 
  }
}


/* Executes once all data is received..then we convert 8bit to 16bit in this loop */
void regen_trip_data() 
{
  unsigned int i,j=0; unsigned int m=0;
  
 //tripdata_16bit->String to be sent to INTERNET //tripdata_8bit ->raw serial data received from UART
  
  for(i=0;i<total_buffer_size;i+=2)
  {
    //We need to convert the current into float. This will first make 8bit to 16bit and to float.
    
    tripdata_16Bit[j]=((((uint16_t)tripdata_8Bit[i]<<8)&0xFF00) + ((uint16_t)tripdata_8Bit[i+1]&0x00FF));
    if(j==7 || j==17 || j==27 || j==37 || j==47) 
      {
        //tripdata_16Bit[j]=((((uint16_t)tripdata_8Bit[i]<<8)&0xFF00) + ((uint16_t)tripdata_8Bit[i+1]&0x00FF));
        I[m]=(float)tripdata_16Bit[j]/100.00;
        m++;
      } 
//      else{
//           tripdata_16Bit[j]=((((uint16_t)tripdata_8Bit[i]<<8)&0xFF00) + ((uint16_t)tripdata_8Bit[i+1]&0x00FF));
//        }   
    //Serial.print(j); Serial.print(": "); Serial.println(tripdata_16Bit[j]);
    j++;
  }
 data_ready=true;
 stringcomplete=false; 
 total_buffer_size=0;
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2);
  Serial.println("UART2 Txd is on pin: 17");
  Serial.println("UART2 Rxd is on pin: 16");
  Serial.println("UART2 Ready");
  InString.reserve(200);

  //Timer Setup
  timer = timerBegin(0, 80, true);  // timer 0, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 80 -> 1000 ns = 1 us, countUp
  timerAttachInterrupt(timer, &onTimer, true); // edge (not level) triggered  /*
  timerAlarmWrite(timer, 2000, true); // 1000 * 1 us = 1 ms, autoreload true
  
  // Initialize the output variables as outputs
  pinMode(LED, OUTPUT);

  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  
  // Uncomment and run it once, if you want to erase all the stored information (it will erase EEPROM)
  
  //wifiManager.resetSettings();
  
  // set custom ip for portal
  //wifiManager.setAPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  // fetches ssid and pass from eeprom and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "NiTech"
  // and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("NiTech");
  // or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();
  
  // if you get here you have connected to the WiFi
  Serial.println("Connected.");
  
  server.begin();
  // Set outputs to LOW
  digitalWrite(LED, LOW);
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
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
              Serial.println("GPIO 2 on");
              output5State = "on";
              digitalWrite(LED, HIGH);
            } else if (header.indexOf("GET /2/off") >= 0) {
              Serial.println("GPIO 2 off");
              output5State = "off";
              digitalWrite(LED, LOW);
            } else if (header.indexOf("GET /TE/data") >= 0) {
              //Serial.println("Sending 0x55 to STM for getting trip events");
              Serial2.write(0x55);     // Send 0x55 to STM32G0 (Stabilizer) for getting Trip Events
              serialEvent2();
            } 
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #2874A6; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #CB4335;}");
            client.println(".table, th, td {border: 1px solid black; border-collapse: collapse; text-align: center;}");
            client.println("</style></head>");
            
            // Web Page Heading
            client.println("<body><h1 style=color:navy;>National Infotech</h1>");
            
            client.println("<h2 style=color:navy;>A Way to Power Electronics and Embeddded System Solutions...</h2>");
            client.println("<h3>Stabilizer Data</h3>");
            
            // Display current state, and ON/OFF buttons for GPIO 2  
            client.println("<p>GPIO 2 - State " + output5State + "</p>");
            // If the output5State is off, it displays the ON button       
            if (output5State=="off") {
              client.println("<p><a href=\"/2/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/2/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display Error History tag and button  
                           
            client.println("<p><a href=\"/TE/data\"><button class=\"button\">Trip Events</button></a></p>");
            client.println("<p><b>Event History:</b></p>");
           
            if(stringcomplete) // If all data is received from UART within 10ms. 
                 {
                   regen_trip_data();                            
                  }
         if(data_ready) //data_ready tag -- 8bit to 16bit conversion complete DISPLAY TABLE.
               {
                  data_ready=false;
               client.println("<table class=center; style=width:100%;>");
                  //Title Row
                  client.println("<tr>    <th>No.</th>    <th>Time</th>    <th>Date</th> <th>Vin</th> <th>Vout</th> <th>I</th>  <th>Ie</th> <th>Ve</th> </tr>");
                  //Row 1 for Event 1
                    client.println("<tr>");
                        client.print("<td>1.</td>");
                        client.println("<td>" + String(tripdata_16Bit[0]) + ":" + String(tripdata_16Bit[1])+ "</td>");
                        client.println("<td>" + String(tripdata_16Bit[2]) + "/" + String(tripdata_16Bit[3])+ "/" +String(tripdata_16Bit[4]) + "</td>");
                        client.println("<td>" + String(tripdata_16Bit[5])+ "</td>");
                        client.println("<td>" + String(tripdata_16Bit[6])+ "</td>");
                        client.println("<td>" + String(I[0])+ "</td>");
                        client.println("<td>" + String(tripdata_16Bit[8])+ "</td>");
                        client.println("<td>" + String(tripdata_16Bit[9])+ "</td>");
                    client.println("</tr>");
                   //Row 2 for Event 2
                    client.println("<tr>");
                        client.print("<td>2.</td>");
                        client.println("<td>" + String(tripdata_16Bit[10]) + ":" + String(tripdata_16Bit[11])+ "</td>");
                        client.println("<td>" + String(tripdata_16Bit[12]) + "/" + String(tripdata_16Bit[13])+ "/" +String(tripdata_16Bit[14]) + "</td>");
                        client.println("<td>" + String(tripdata_16Bit[15])+ "</td>");
                        client.println("<td>" + String(tripdata_16Bit[16])+ "</td>");
                        client.println("<td>" + String(I[1])+ "</td>");
                        client.println("<td>" + String(tripdata_16Bit[18])+ "</td>");
                        client.println("<td>" + String(tripdata_16Bit[19])+ "</td>");
                    client.println("</tr>");
                    //Row 3 for Event 3
                    client.println("<tr>");
                        client.print("<td>3.</td>");
                        client.println("<td>" + String(tripdata_16Bit[20]) + ":" + String(tripdata_16Bit[21])+ "</td>");
                        client.println("<td>" + String(tripdata_16Bit[22]) + "/" + String(tripdata_16Bit[23])+ "/" +String(tripdata_16Bit[24]) + "</td>");
                        client.println("<td>" + String(tripdata_16Bit[25])+ "</td>");
                        client.println("<td>" + String(tripdata_16Bit[26])+ "</td>");
                        client.println("<td>" + String(I[2])+ "</td>");
                        client.println("<td>" + String(tripdata_16Bit[28])+ "</td>");
                        client.println("<td>" + String(tripdata_16Bit[29])+ "</td>");
                    client.println("</tr>");
                    //Row 4 for Event 4
                    client.println("<tr>");
                        client.print("<td>4.</td>");
                        client.println("<td>" + String(tripdata_16Bit[30]) + ":" + String(tripdata_16Bit[31])+ "</td>");
                        client.println("<td>" + String(tripdata_16Bit[32]) + "/" + String(tripdata_16Bit[33])+ "/" +String(tripdata_16Bit[34]) + "</td>");
                        client.println("<td>" + String(tripdata_16Bit[35])+ "</td>");
                        client.println("<td>" + String(tripdata_16Bit[36])+ "</td>");
                        client.println("<td>" + String(I[3])+ "</td>");
                        client.println("<td>" + String(tripdata_16Bit[38])+ "</td>");
                        client.println("<td>" + String(tripdata_16Bit[39])+ "</td>");
                    client.println("</tr>");
                    //Row 5 for Event 5
                    client.println("<tr>");
                        client.print("<td>5.</td>");
                        client.println("<td>" + String(tripdata_16Bit[40]) + ":" + String(tripdata_16Bit[41])+ "</td>");
                        client.println("<td>" + String(tripdata_16Bit[42]) + "/" + String(tripdata_16Bit[43])+ "/" +String(tripdata_16Bit[44]) + "</td>");
                        client.println("<td>" + String(tripdata_16Bit[45])+ "</td>");
                        client.println("<td>" + String(tripdata_16Bit[46])+ "</td>");
                        client.println("<td>" + String(I[4])+ "</td>");
                        client.println("<td>" + String(tripdata_16Bit[48])+ "</td>");
                        client.println("<td>" + String(tripdata_16Bit[49])+ "</td>");
                    client.println("</tr>");
                    
                client.println("</table>");
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