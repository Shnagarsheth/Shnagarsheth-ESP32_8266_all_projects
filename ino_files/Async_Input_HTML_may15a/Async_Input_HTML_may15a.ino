/*******
*  Asyncr Web Server
*  station mode
*  A simple server implementation showing how to:
*  serve static messages
*  read GET and POST parameters
**********/

#include <Arduino.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "TP-LINK_39FA";
const char* password = "Qwertyuiop";

const char* PARAM_INPUT_1 = "Date";
const char* PARAM_INPUT_2 = "Time";

/**** HTML web page to handle 2 input fields (Date, time)****/
//below string data stored in flash memory 

const char index_html[] PROGMEM = R"rawliteral(               
<!DOCTYPE HTML><html>
   <body><center>
  <h2> National Infotech</h2>
  <h3> Solar Panel Cleaner </h3>
  <form action="/get">
    Date: <input type="date" name="Date">
    <input type="submit" value="Submit">
  </form><br>
  <form action="/get">
    Time: <input type="time" name="Time">
    <input type="submit" value="Submit">
  </form><br>
  
</center></body></html>)rawliteral";
/***************         *************/
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");  //route not found handling function,so return to the client a 404 HTTP response code. As content, we will return a plain text string with a message indicating that the content was not found
}

void setup() {
  Serial.begin(115200);
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
    String inputMessage;
    String inputParam;
    // GET Date value on <ESP_IP>/get?Date=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
    }
    // GET Time value on <ESP_IP>/get?time=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_2)) {
      inputMessage = request->getParam(PARAM_INPUT_2)->value();
      inputParam = PARAM_INPUT_2;
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    int onHours=inputMessage.toInt();
    String inputMessage_Minutes=inputMessage.substring(3);
    int onMinutes=inputMessage_Minutes.toInt();
    
    Serial.print("onHours: ");
    Serial.println(onHours);
    Serial.print("onMinutes: ");
    Serial.println(onMinutes);
    Serial.println(inputMessage);
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                     + inputParam + ") with value: " + inputMessage +
                                     "<br><a href=\"/\">Return to Home Page</a>");
  });
  server.onNotFound(notFound);
  server.begin();
}

void loop() {
  
}
