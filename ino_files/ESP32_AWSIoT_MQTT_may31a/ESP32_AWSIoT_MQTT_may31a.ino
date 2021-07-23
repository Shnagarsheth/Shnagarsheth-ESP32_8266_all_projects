#include <WiFi.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


//WiFi Credintials
const char* ssid = "*********";                  // Your wifi Name       
const char* password = "*********";             // Your wifi Password

//Define LED
#define Led_OnBoard 2

//Stabilizer Fake Data
int entryNo = 1, Vin = 285, Vout = 220, Ie = 0, Ve = 0; 
String stabilizerID = "Nitech_1";
float Current = 3.5;

unsigned long lastPublish;
int msgCount;
volatile uint8_t ledStatus;


//Define IST  (Indian Standard Time) offset & string
const int utcOffsetInSeconds = 19800;
String formattedDate, formattedTime, dayStamp, timeStamp;

// Device data endpoint is used only for IoT device control and communications.
const char* awsEndpoint = "*****************"; //Enter your AWS End-point

//Certificate

const char* certificatePemCrt = \
"-----BEGIN CERTIFICATE-----\n" \
"*************************************\n"\
"-----END CERTIFICATE-----\n";

const char* privatePemKey=\
"-----BEGIN RSA PRIVATE KEY-----\n" \
"*************************************\n"\
"-----END RSA PRIVATE KEY-----\n";

const char* rootCA=\
"-----BEGIN CERTIFICATE-----\n" \
"*************************************\n"\
"-----END CERTIFICATE-----\n";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

WiFiClientSecure wiFiClient;
void msgReceived(char* topic, byte* payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient);

void wifiSetup(){  
    Serial.print("Connecting to "); Serial.print(ssid);
    WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
    delay(1000);
    WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
    WiFi.begin(ssid, password);     //Connect to your WiFi router
    
    while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(Led_OnBoard, HIGH);
    delay(250);
    Serial.print(".");
    digitalWrite(Led_OnBoard, LOW);
    delay(250);
    }
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());    
  }

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); 
  pinMode(Led_OnBoard, OUTPUT); 
  Serial.printf("SDK version: %s \n",ESP.getSdkVersion());

  wifiSetup();
  digitalWrite(Led_OnBoard, LOW);
  timeClient.begin();

  wiFiClient.setCACert(rootCA);
  wiFiClient.setCertificate(certificatePemCrt);
  wiFiClient.setPrivateKey(privatePemKey);
}

void pubSubCheckConnect() {
  if ( !pubSubClient.connected()) {
    Serial.print("PubSubClient connecting to: "); Serial.print(awsEndpoint);
    while ( !pubSubClient.connected()) {
      Serial.print(".");
      pubSubClient.connect("ESPthingXXXX");
      delay(1000);
    }
    Serial.println("connected");
    pubSubClient.subscribe("inTopic");
  }
  pubSubClient.loop();
}


void loop() {
  // put your main code here, to run repeatedly:
  timeClient.update();
  pubSubCheckConnect();
  getCurrentTime();
    
  char fakeData[256];
  //sprintf(fakeData, "{\"No.\":%d, \"Vin\":%d, \"Vout\": %d, \"Current\": %f}", entryNo, Vin, Vout, Current); 
  //sprintf(fakeData, "{\"Time\":%s, \"Date\":%s, \"Vin\":%d, \"Vout\": %d, \"Current\": %f}",  timeStamp, dayStamp, Vin, Vout, Current);  
  sprintf(fakeData, "{\"StabilizerID\":\"%s\", \"No.\":%d, \"Time\":\"%s\", \"Date\":\"%s\", \"Vin\":%d, \"Vout\": %d, \"Current\": %f, \"Ie\": %d, \"Ve\": %d}", stabilizerID, entryNo, timeStamp, dayStamp, Vin, Vout, Current, Ie, Ve);
 /*
  * Incase you use ArduinoJSON.h
  * char fakeData[256];
  * serializeJson(doc, fakeData);
  * client.publish("outTopic", fakeData);
  */
  
  //Publish to the topic on AWS every 10 second
  if (millis() - lastPublish > 10000) {
      //String msg = String("Hello from ESP32: ") + ++msgCount;
      // boolean rc = pubSubClient.publish("outTopic", msg.c_str());
      boolean rc = pubSubClient.publish("stabilizer/tripEvents", fakeData);
      Serial.print("Published, rc="); Serial.print( (rc ? "OK: " : "FAILED: ") );
      Serial.println(fakeData);
      Vin+=1; Vout+=1; Current+=1.2; entryNo+=1;
      lastPublish = millis();
     } 
   if (ledStatus == 1){
    digitalWrite(Led_OnBoard, HIGH);
    }
   else{
    digitalWrite(Led_OnBoard, LOW);
    }
}

void getCurrentTime(){
  formattedDate = timeClient.getFormattedDate();
  formattedTime = timeClient.getFormattedTime();
  
  // Extract date
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  //Serial.print("DATE: ");
  //Serial.println(dayStamp);
  
  // Extract time
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  //Serial.print("HOUR: ");
  //Serial.println(timeStamp);
  }

/*  
 *   This subrotuine is for msgReceived from the MQTT broker
 */
void msgReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on "); Serial.print(topic); Serial.print(": ");
  StaticJsonDocument<256> doc;
  deserializeJson(doc, payload, length); 
//  deserializationError err= deserializeJson(doc, payload, length);
//  if(err){
//    Serial.print("Error: ");
//    Serial.println(err.c_str());
//    return;
//    }

  //getting status of LED from the payload received  
    ledStatus = doc["LED"];
  
//  for (int i = 0; i < length; i++) {
//    Serial.print((char)payload[i]);
   /* 
    *  In case you use ArduinoJSON library
    * StaticJsonDocument<256> doc;
    * deserializeJson(doc, payload, length);  
    */    
  //}
  Serial.println(ledStatus);
}
