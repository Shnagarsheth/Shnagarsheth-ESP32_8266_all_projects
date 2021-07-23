#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

int Led_OnBoard = 2;                  // Initialize the Led_OnBoard 

//----------I assume you have successfully accessed the sensors. And below is the variable. I made it an arbitrary value.
int Vin1     = 285;
int Vout1     = 220;
float Current1   = 3.5;

String Vin, Vout, Current, postData;
//----------

const char* ssid = "TP-LINK_39FA";                  // Your wifi Name       
const char* password = "Qwertyuiop";          // Your wifi Password

const char *host = "http://viral.palsoft.org/"; //Your pc or server (database) IP, example : 192.168.0.0 , if you are a windows os user, open cmd, then type ipconfig then look at IPv4 Address.

void setup() {
  // put your setup code here, to run once:
  delay(1000);
  pinMode(Led_OnBoard, OUTPUT);       // Initialize the Led_OnBoard pin as an output
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(Led_OnBoard, LOW);
    delay(250);
    Serial.print(".");
    digitalWrite(Led_OnBoard, HIGH);
    delay(250);
  }

  digitalWrite(Led_OnBoard, HIGH);
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.println("Connected to Network/SSID");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
}

void loop() {
  // put your main code here, to run repeatedly:
  WiFiClient client; //Added Client to work with new library.
  HTTPClient http;    //Declare object of class HTTPClient

  //-------------------------------------------to send data to the database
  //String Vin, Vout, Current, postData;
  Vin = String(Vin1);   //String to interger conversion
  Vout = String(Vout1);   //String to interger conversion
  Current = String(Current1);   //String to interger conversion
  
  postData = "&Vin=" + Vin + "&Vout=" + Vout + "&Current=" + Current;


  
  http.begin(client, "http://viral.palsoft.org/stabilizerAPI/InsertDB.php");               //Specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");                  //Specify content-type header
 
  int httpCode = http.POST(postData);   //Send the request
  String payload = http.getString();    //Get the response payload
  //-------------------------------------------
  
  //Serial.println("LDR Value=" + ldrvalue);
  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload
  Serial.println("Vin= " + Vin + " Vout= " + Vout + " Current= " + Current);
  
  http.end();  //Close connection

  delay(4000);  //Here there is 4 seconds delay plus 1 second delay below, so Post Data at every 5 seconds
  Vin1+=1; Vout1+=1; Current1+=1.2;
  digitalWrite(Led_OnBoard, LOW);
  delay(1000);
  digitalWrite(Led_OnBoard, HIGH);
}
