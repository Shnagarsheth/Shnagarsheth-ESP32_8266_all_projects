/*
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/


#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "TP-LINK_39FA";
const char* password = "Qwertyuiop";
const char* mqtt_server = "broker.mqtt-dashboard.com";
const char *topic = "esp8266/test";
//const char *mqtt_username = "emqx";
//const char *mqtt_password = "public";
//const int mqtt_port = 1883;
WiFiClient espClient;
PubSubClient client(espClient); //allows you to send and receive MQTT message
long lastMsg = 0;
char msg[50];
int value = 0;

void setup() 
{
  pinMode(16, OUTPUT);     // Initialize the BUILTIN_LED/16 pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883); //setup the MQTT server on MQTT portNumber
  client.setCallback(callback);   //payload  /outT
}

        void callback(char* topic, byte* payload, unsigned int length) 
        { 
          Serial.print("Message arrived in [");
          Serial.print(topic);
          Serial.print("] ");
          for (int i = 0; i < length; i++) {     //received payloads are strings
            Serial.print((char)payload[i]);
          }
          Serial.println();
        
          // Switch on the LED if an 1 was received as first character
          if ((char)payload[0] == '1') 
                    {
                     digitalWrite(16, LOW);   // Turn the LED ON (Note that LOW is the voltage level
                               // but actually the LED is on; this is because it is acive low on the ESP-01)
                    } 
            else {
                   digitalWrite(16, HIGH);  // Turn the LED off by making the voltage HIGH
                 }
        }
        
        

void loop() {

  if (!client.connected()) { //for reconect
    reconnect();             /****/
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {  //publishes "hello world" to the topic "outTopic" every two seconds
    lastMsg = now;
    ++value;
    snprintf (msg, 75, "hello world #%ld", value); //buf,buff_size, 
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
  }
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  //randomSeed(micros()); //random number
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
        /***** Loop until we're reconnected*****/
         while (!client.connected()) {
            Serial.print("Attempting MQTT connection...");
            
            String clientId = "ESP8266Client-"; // Create a random client ID
                     //  clientId += String(random(0xffff), HEX); //client_id += String(WiFi.macAddress());
            
           if (client.connect(clientId.c_str())) // Attempt to connect  // if (client.connect(client_id, mqtt_username, mqtt_password))
           {
              Serial.println("connected");
                // Once connected, publish an announcement...
             client.publish(topic, "hello MQTTBrok");
             client.subscribe(topic);    //...subscribe                                   /**Nchange**/
               // client.subscribe("inTopic");   //*orignl*/...and re-subscribes to the topic "inTopic", printing out any messages
             } 
             else {
                  Serial.print("failed, reconnect="); //reconnect fail
                  Serial.print(client.state());
                  Serial.println(" try again in 5 seconds");
                  // Wait 5 seconds before retrying
                  delay(5000);
                  }
          }
        }
