#include <WiFi.h>

//WiFi Credintials
const char* ssid = "TP-LINK_39FA";                  // Your wifi Name       
const char* password = "Qwertyuiop";          // Your wifi Password

//Define LED
#define Led_OnBoard 2

byte mac[6];
char mac_Id[18];


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

  wifiSetup();
  digitalWrite(Led_OnBoard, LOW);

  // Getting MAC address of the ESP32==============================
  WiFi.macAddress(mac);
  snprintf(mac_Id, sizeof(mac_Id), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.println(mac_Id);
  //===============================================================
}


void loop() {
  // put your main code here, to run repeatedly:

}
