/*****
 * Name: Shaival Nagarsheth
 * Description: This sketch provides the UART Data from the stabilizer (Trip History/Trip Events). 
 *              On a button press in a web page, the trip history is updated. On button press 0x55 is sent (through UART) to 
 *              stabilizer controller (i.e. STM32G030), which in turn provides the trip data on uart.
 *              Blink LED is also embedded. 
 *              We first convert the 8bit received data into 16bit data.
 *              Moreover, Iin has to coverted in float value and devide by 100 in order to display it corrrectly.                 
 * *****/
#include <WiFi.h>
//#include <DNSServer.h>
//#include <WebServer.h>
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output5State = "off";
String output4State = "off";

// Assign output variables to GPIO pins
const int LED = 2;
const int output4 = 4;

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
  timerAlarmWrite(timer, 1000, true); // 1000 * 1 us = 1 ms, autoreload true
  
  // Initialize the output variables as outputs
  pinMode(LED, OUTPUT);
  pinMode(output4, OUTPUT);


  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  
  // Uncomment and run it once, if you want to erase all the stored information (it will erase EEPROM)
  //wifiManager.resetSettings();
  
  // set custom ip for portal
  //wifiManager.setAPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  // fetches ssid and pass from eeprom and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("AutoConnectAP");
  // or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();
  
  // if you get here you have connected to the WiFi
  Serial.println("Connected.");
  
  server.begin();
  // Set outputs to LOW
  digitalWrite(LED, LOW);
  digitalWrite(output4, LOW);
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
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
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("GPIO 2 on");
              output5State = "on";
              digitalWrite(LED, HIGH);
            } else if (header.indexOf("GET /5/off") >= 0) {
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
            client.println(".button2 {background-color: #CB4335;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1 style=color:navy;>National Infotech</h1>");
            
            client.println("<h2 style=color:navy;>A Way to Power Electrconics and Embeddded System Solutions...</h2>");
            client.println("<h3>Stabilizer Data</h3>");
            
            // Display current state, and ON/OFF buttons for GPIO 2  
            client.println("<p>GPIO 2 - State " + output5State + "</p>");
            // If the output5State is off, it displays the ON button       
            if (output5State=="off") {
              client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display Error History tag and button  
                           
            client.println("<p><a href=\"/TE/data\"><button class=\"button\">Trip Events</button></a></p>");
            client.println("<p>Trip Events:</p>");
            if(stringcomplete) // If all data is received from UART within 10ms. 
                 {
                   regen_trip_data();                            
                  }
             if(data_ready) //data_ready tag -- 8bit to 16bit conversion complete DISPLAY TABLE.
                {
                  data_ready=false;
                  //EVENT 1
                  client.println("<p><b>Event 1:</b></p>");   
               // client.println("<p>Time:" + String(tripdata_16Bit[0]) + "</p>"); //client.print(":" + tripdata_16Bit[1]); client.print("</p>");
                  //+ ':' + tripdata_16Bit[1] + 'Date:' + tripdata_16Bit[2] + '/' + tripdata_16Bit[3] + '/' + tripdata_16Bit[4] + 'Vin:' + tripdata_16Bit[5] + 'Vout:' + tripdata_16Bit[6] + 'I:' + tripdata_16Bit[7] + 'Ie:' + tripdata_16Bit[8] + 'Ve:' + tripdata_16Bit[9] + "<br></p>");
                 client.println("<p><b>Time: </b>" + String(tripdata_16Bit[0]) + ":" + String(tripdata_16Bit[1]) + "<b> Date: </b>" + String(tripdata_16Bit[2]) + "/" + String(tripdata_16Bit[3]) + "/" + String(tripdata_16Bit[4]) + "<b> Vin: </b>" + String(tripdata_16Bit[5]) +"V "+ "<b> Vout: </b>" + String(tripdata_16Bit[6]) + "V" + "<b> I: </b>" + String(I[0]) +"A"+ "<b> Ie: </b>" + String(tripdata_16Bit[8]) +"mA"+ "<b> Ve: </b>" + String(tripdata_16Bit[9]) + "V"+"</p>");
                    //EVENT 2                  
                  client.println("<p><b>Event 2:</b></p>");
//                  client.println("<p>Time:" + tripdata_16Bit[10] + ':' + tripdata_16Bit[11] + 'Date:' + tripdata_16Bit[12] + '/' + tripdata_16Bit[13] + "/" + tripdata_16Bit[14] + "Vin:" + tripdata_16Bit[15] + "Vout:" + tripdata_16Bit[16] + "I:" + tripdata_16Bit[17] + "Ie:" + tripdata_16Bit[18] + "Ve:" + tripdata_16Bit[19] + "</p>");
                 client.println("<p><b>Time: </b>" + String(tripdata_16Bit[10]) + ":" + String(tripdata_16Bit[11]) + "<b> Date: </b>" + String(tripdata_16Bit[12]) + "/" + String(tripdata_16Bit[13]) + "/" + String(tripdata_16Bit[14]) + "<b> Vin: </b>" + String(tripdata_16Bit[15]) +"V "+ "<b> Vout: </b>" + String(tripdata_16Bit[16]) + "V" + "<b> I: </b>" + String(I[1]) +"A"+ "<b> Ie: </b>" + String(tripdata_16Bit[18]) +"mA"+ "<b> Ve: </b>" + String(tripdata_16Bit[19]) + "V"+"</p>");
//                  //EVENT 3
                  client.println("<p><b>Event3:</b></p>");
//                  client.println("<p>Time:" + tripdata_16Bit[20] + ':' + tripdata_16Bit[21] + 'Date:' + tripdata_16Bit[22] + '/' + tripdata_16Bit[23] + "/" + tripdata_16Bit[24] + "Vin:" + tripdata_16Bit[25] + "Vout:" + tripdata_16Bit[26] + "I:" + tripdata_16Bit[27] + "Ie:" + tripdata_16Bit[28] + "Ve:" + tripdata_16Bit[29] + "</p>");
                 client.println("<p><b>Time: </b>" + String(tripdata_16Bit[20]) + ":" + String(tripdata_16Bit[21]) + "<b> Date: </b>" + String(tripdata_16Bit[22]) + "/" + String(tripdata_16Bit[23]) + "/" + String(tripdata_16Bit[24]) + "<b> Vin: </b>" + String(tripdata_16Bit[25]) +"V "+ "<b> Vout: </b>" + String(tripdata_16Bit[26]) + "V" + "<b> I: </b>" + String(I[2]) +"A"+ "<b> Ie: </b>" + String(tripdata_16Bit[28]) +"mA"+ "<b> Ve: </b>" + String(tripdata_16Bit[29]) + "V"+"</p>");
//                  //Event 4
                  client.println("<p><b>Event4:</b></p>");
//                  client.println("<p>Time:" + tripdata_16Bit[30] + ':' + tripdata_16Bit[31] + 'Date:' + tripdata_16Bit[32] + '/' + tripdata_16Bit[33] + "/" + tripdata_16Bit[34] + "Vin:" + tripdata_16Bit[35] + "Vout:" + tripdata_16Bit[36] + "I:" + tripdata_16Bit[37] + "Ie:" + tripdata_16Bit[38] + "Ve:" + tripdata_16Bit[39] + "</p>");
                 client.println("<p><b>Time: </b>" + String(tripdata_16Bit[30]) + ":" + String(tripdata_16Bit[31]) + "<b> Date: </b>" + String(tripdata_16Bit[32]) + "/" + String(tripdata_16Bit[33]) + "/" + String(tripdata_16Bit[34]) + "<b> Vin: </b>" + String(tripdata_16Bit[35]) +"V "+ "<b> Vout: </b>" + String(tripdata_16Bit[36]) + "V" + "<b> I: </b>" + String(I[3]) +"A"+ "<b> Ie: </b>" + String(tripdata_16Bit[38]) +"mA"+ "<b> Ve: </b>" + String(tripdata_16Bit[39]) + "V"+"</p>");
//                  //EVENT 5
                  client.println("<p><b>Event5:</b></p>");
//                  client.println("<p>Time:" + tripdata_16Bit[40] + ':' + tripdata_16Bit[41] + 'Date:' + tripdata_16Bit[42] + '/' + tripdata_16Bit[43] + "/" + tripdata_16Bit[44] + "Vin:" + tripdata_16Bit[45] + "Vout:" + tripdata_16Bit[46] + "I:" + tripdata_16Bit[47] + "Ie:" + tripdata_16Bit[48] + "Ve:" + tripdata_16Bit[49] + "</p>");
                 client.println("<p><b>Time: </b>" + String(tripdata_16Bit[40]) + ":" + String(tripdata_16Bit[41]) + "<b> Date: </b>" + String(tripdata_16Bit[42]) + "/" + String(tripdata_16Bit[43]) + "/" + String(tripdata_16Bit[44]) + "<b> Vin: </b>" + String(tripdata_16Bit[45]) +"V "+ "<b> Vout: </b>" + String(tripdata_16Bit[46]) + "V" + "<b> I: </b>" + String(I[4]) +"A"+ "<b> Ie: </b>" + String(tripdata_16Bit[48]) +"mA"+ "<b> Ve: </b>" + String(tripdata_16Bit[49]) + "V"+"</p>");
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
    if(j==7 || j==17 || j==27 || j==37 || j==47)
      {
        tripdata_16Bit[j]=((((uint16_t)tripdata_8Bit[i]<<8)&0xFF00) + ((uint16_t)tripdata_8Bit[i+1]&0x00FF));
        I[m]=(float)tripdata_16Bit[j]/100.00;
        m++;
      } 
      else{
           tripdata_16Bit[j]=((((uint16_t)tripdata_8Bit[i]<<8)&0xFF00) + ((uint16_t)tripdata_8Bit[i+1]&0x00FF));
        }   
    Serial.print(j); Serial.print(": "); Serial.println(tripdata_16Bit[j]);
    j++;
  }
//  I[0]= (float)tripdata_16Bit[7]/100.00;
//  I[1]= (float)tripdata_16Bit[17]/100.00;
//  I[2]= (float)tripdata_16Bit[27]/100.00;
//  I[3]= (float)tripdata_16Bit[37]/100.00;
//  I[4]= (float)tripdata_16Bit[47]/100.00; 

  data_ready=true;
  stringcomplete=false; 
}
