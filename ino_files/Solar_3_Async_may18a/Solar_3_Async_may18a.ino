/*
 * Project: Solar Panel Cleaner
 * Written By: Shaival Nagarsheth 
 * Version: 1.2
 * Detailed Task: 1. Make Async Webserver. 
 *                2. Form for getting date & time from the user to start the cleaning pump.
 *                3. Creating multiple pages for navigation from main to daughter pages.
 *                4. Button for Instant START and Instant OFF.
 *                5. Getting NTP server Time as well continuously
 * Note: 
 */
 
#include <Arduino.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

AsyncWebServer server(80);

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "TP-LINK_39FA";
const char* password = "Qwertyuiop";

// Characters for Date and Time
const char* PARAM_INPUT_1 = "Date";
const char* PARAM_INPUT_2 = "Time";

// Assign output variables to GPIO pins
const int output1 = 16;
const int output2 = 2;

//Setting Time for Switching on the Cleaning Pump
int onHours, onMinutes;

//Declaration of string to get date and time
String onTime, onDate,inputMessage, inputParam1, inputParam2;

//NTP Server Variables
const long utcOffsetInSeconds = 19800; //Offset in seconds for UTC: +5:30, which is IST (Indian Standard Time).

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

String currentDate;

//Declaring time and date variables
int currentHours = 0, currentMinutes=0 , currentSeconds=0,currentMonthDay =0, currentMonth = 0, currentYear =0;

//Declaring flag for PUMP ON/OFF status. 
uint8_t pumpOnFlag=0, printFlag =0, instantOnFlag=0, instantOffFlag=0;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

/**** Main HTML Webpage ****/
//below string data stored in flash memory 

const char index_html[] PROGMEM = R"rawliteral(               
<!DOCTYPE HTML><html>
<head><meta name="viewport" content="width=device-width, initial-scale=1">
<style>html {font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}
      .button {background-color: #2874A6; border: none; color: white; padding: 12px 30px;text-decoration: none; font-size: 16px; margin: 2px; border-radius: 4px; cursor: pointer;}
      .button2{background-color: #CB4335; border: none; color: white; padding: 12px 30px;text-decoration: none; font-size: 16px; margin: 2px; border-radius: 4px; cursor: pointer;}
      .button3{background-color: #ffc900; border: none; color: white; padding: 12px 30px;text-decoration: none; font-size: 16px; margin: 2px; border-radius: 4px; cursor: pointer;}
      .button:hover{background-color: #071b5d;}
      .button2:hover{background-color: #75290f;}
      .button3:hover{background-color: #dbb00d;}
      input[type=submit] {background-color:#4caf50; color:white; padding: 10px 60px; font-size: 16px; margin: 2px; border-radius: 4px;cursor: pointer;}
      input[type=submit]:hover {background-color:#45a049;}
</style></head>

  <body><center>
  <h1 style=color:navy;>National Infotech</h1>
  <h2 style=color:navy;>A Way to Power Electronics and Embedded System Solutions...</h2>
  <h3>Solar Panel Cleaner</h3>

  <form action="/get">
    Date: <input type="date" name="Date"><br><br>
    Time: <input type="time" name="Time"><br><br>
    <input type="submit" value="Set"><br>    
  </form>
  OR<br>
  <a href="/on"><button class="button"> Instant Start </button></a>
  <a href="/off"><button class="button button2"> Instant OFF </button></a><br>
  <a href="/prev"><button class="button button3"> Previous Clean History </button></a>
  
</center></body></html>)rawliteral";
/***************         *************/

//Second Page
//Third Page
//Fourth Page


void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");  //route not found handling function,so return to the client a 404 HTTP response code. As content, we will return a plain text string with a message indicating that the content was not found
}

void setup() {
  Serial.begin(115200);
  pinMode(output1, OUTPUT);
  digitalWrite(output1, HIGH);
  WiFi.mode(WIFI_STA);  
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/get?Date=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    //String onTime, onDate,inputMessage, inputParam1, inputParam2;
    
    // GET Date and Time value on <ESP_IP>/get?Date=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1) || request->hasParam(PARAM_INPUT_2)) {
      onDate = request->getParam(PARAM_INPUT_1)->value();
      onTime = request->getParam(PARAM_INPUT_2)->value();
      inputParam1 = PARAM_INPUT_1;
      inputParam2 = PARAM_INPUT_2;
    }
    else {
      inputMessage = "No message sent";
      inputParam1 = "none";
    }
    onHours=onTime.toInt();
    String onTime_Minutes=onTime.substring(3);
    onMinutes=onTime_Minutes.toInt();
    Serial.print("onHours: ");
    Serial.println(onHours);
    Serial.print("onMinutes: ");
    Serial.println(onMinutes);
    Serial.println(onDate);
    request->send(200, "text/html", "Your Date & Time is set for cleaning <br> <b> Date: </b>" + onDate + "<br> <b> Time: </b>" + onTime + "<br><a href=\"/\">Return to Home Page</a>");
  });

  server.on("/on", HTTP_GET, [] (AsyncWebServerRequest *request) {
    instantOnFlag=1;
    request->send(200,"text/html","Cleaning has been STARTED, instanly.<br><a href=\"/\">Return to Home Page</a>");
  });

  server.on("/off", HTTP_GET, [] (AsyncWebServerRequest *request) {
    instantOffFlag=1;
    request->send(200,"text/html","Cleaning has been STOPPED, instanly.<br><a href=\"/\">Return to Home Page</a>");
  });

  server.on("/prev", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send(200,"text/html","Here is your clean history<br> Date: " + onDate + "<br>Time: "+ onTime + "<br><a href=\"/\">Return to Home Page</a>");
  });
  
  server.onNotFound(notFound);
  server.begin();
  timeClient.begin();
}

void loop() {
  timeClient.update(); //to get the current date and time from the NTP server.
  currentSeconds = timeClient.getSeconds();
  currentHours = timeClient.getHours();
  currentMinutes = timeClient.getMinutes();
  unsigned long epochTime = timeClient.getEpochTime();
  
  //Creating Time Structure 
  struct tm *ptm = gmtime((time_t *)&epochTime);
  currentMonthDay =ptm->tm_mday;
  currentMonth = ptm->tm_mon+1;
  currentYear = ptm->tm_year+1900; 
  currentDate = String(currentMonthDay) + "-" + String(currentMonth) + "-" + String(currentYear); 
  int cdate=currentDate.toInt();

  //MAIN LOGIC FOR SWITCHING ON THE PUMP
  if ((currentHours >= 7) && (currentHours <= 20) && (pumpOnFlag==0)){
     if(((onHours == currentHours) && (onMinutes == currentMinutes) && (currentSeconds == 0)) || (instantOnFlag==1)){
        digitalWrite(output1,LOW);
        pumpOnFlag=1; printFlag = 1;             
      }    
    }

  if((printFlag == 1)){
     printFlag =0;instantOnFlag=0;
     Serial.println("Relax! Your Solar Panels are being cleaned.");  
     }
      
  if((pumpOnFlag==1) ){    
    if ((currentMinutes == (1 + onMinutes)) || (instantOffFlag==1) ){
      pumpOnFlag=0;instantOffFlag==0;
      digitalWrite(output1,HIGH);
      Serial.println("Your Solar Panels have been successfully cleaned.");  
      }
    }
  
}
