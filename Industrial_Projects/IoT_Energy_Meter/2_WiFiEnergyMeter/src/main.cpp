/**
 * @file main.cpp
 * @author Shaival Nagarsheth (shaival.nitech@gmail.com)
 * @brief Sending Energy Vitals to MQTTbroker over WiFi. Reading Energy Vitals from ADE7758 over SPI from ESP32.
 * @version 0.1
 * @date 2021-11-17
 *  
 * @copyright Copyright (c) National Infotech 2021
 * 
 */

#include <Arduino.h>
#include "SPI.h"
#include "ADE7758.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Replace the next variables with your SSID/Password combination
const char* ssid = "NITECH";
const char* password = "FACE654321";

// Add your MQTT Broker IP address, example:
const char* mqtt_server = "103.228.78.243";

//Define IST  (Indian Standard Time) offset & string
const int utcOffsetInSeconds = 19800;
String formattedDate, formattedTime, dayStamp, timeStamp;

char energyMeterData[256];
byte mac[6];
char mac_Id[18];
unsigned long lastPublish;

WiFiClient espClient;
PubSubClient pubSubClient(espClient);

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

#define DIV 2
#define DAC1 25
#define WATT_CAL 0.205638806

#define VAR_CAL 20.2
#define VA_CAL 0.2357520278
#define VRMS_CAL 4660.0574712643
#define IRMS_CAL 0.005995
#define FREQ_CAL 20.67

unsigned char defaultValue;
unsigned char mmodeReg = 0x14;
float voltagePhaseB, currentPhaseB;
int entryNo =0;

float Watt_a, Watt_b, Watt_c, wValue;
float Var_a, Var_b, Var_c;
float VA_a, VA_b, VA_c;
float Tension_a, Tension_b, Tension_c;
float Corriente_a, Corriente_b, Corriente_c;
float Frequency;

ADE7758 meter;

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

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void pubSubCheckConnect() {
  if ( !pubSubClient.connected()) {
    Serial.print("PubSubClient connecting to: "); Serial.print(mqtt_server);
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

void setup() {
 
  Serial.begin(115200);
  Serial.println("ADE7758 test. Configurando..");

  setup_wifi();
  pubSubClient.setServer(mqtt_server, 1883);
  // client.setCallback(callback);

  meter.Init();
  meter.setLcycMode(0); // Todos los bits en 0, ver el cpp para mas informacion.
  meter.gainSetup(INTEGRATOR_OFF, FULLSCALESELECT_0_25V, GAIN_1, GAIN_1);
  meter.setupDivs(DIV, DIV, DIV); //Divisor de los valores que se guardan en los registros de potencia
  meter.resetStatus();
  meter.setMaskInterrupts(0x01400);

  /*********Getting MAC address of the ESP32*******/
  WiFi.macAddress(mac);
  snprintf(mac_Id, sizeof(mac_Id), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.println(mac_Id);
 /****************/  
}

/*****************************
		Main Loop
******************************/
void loop() {
  timeClient.update();
  getCurrentTime();
  Serial.print(meter.getMMode(), HEX); Serial.print("---"); Serial.print(meter.getOpMode(), HEX); Serial.print("---"); 
  Serial.print(meter.getWavMode()); Serial.print("---"); Serial.print(meter.getCompMode()); Serial.print("---"); Serial.println(meter.getLcycMode());
  Serial.println("-------------");
 
  ////======================COMIENZO DE MEDICIONES===============================
  Serial.println("Measuring..");
  Serial.println("--------------------------------");
  Tension_b = meter.getBVRMS();
  Corriente_b = (meter.getBIRMS()/ 1000);
  currentPhaseB = (Corriente_b *  IRMS_CAL);
  voltagePhaseB =  (Tension_b / VRMS_CAL);
  Frequency = (meter.getFreq()/20.67);
  Serial.print("Freqency: "); Serial.println(Frequency);
  
  if (meter.setPotLine(PHASE_B, 20)) {
    Watt_b = meter.getWatt(PHASE_B);
    Var_b = meter.getVar(PHASE_B);
    VA_b = meter.getVa(PHASE_B);
  } else {
    Serial.println("There is no zero crossing in Phase B");
  }
  
  Serial.println("--------------Raw DATA:-------------");    
 //  Serial.println("---------------------------");  
  Serial.println("Phase B:");
  Serial.print("V:");  Serial.println( Tension_b, DEC );
  Serial.print("I:");  Serial.println( Corriente_b,DEC );

  Serial.println("--------------------------------------------------------");
  
  Serial.println("-->Corrected values:---------------");
  Serial.println("Phase B:");
  Serial.print("V:");  Serial.println( Tension_b / VRMS_CAL, DEC );
  Serial.print("I:");  Serial.println( ((Corriente_b*IRMS_CAL)/2.045), DEC);
  Serial.println( ((Corriente_b*IRMS_CAL)), DEC);

  pubSubCheckConnect();
  sprintf(energyMeterData, "{\"ControllerID\":\"%s\",\"no\":%d, \"time\":\"%s\", \"date\":\"%s\", \"VRMS\":%f, \"IRMS\": %f, \"Frequency\": %f}", mac_Id, entryNo, timeStamp, dayStamp, voltagePhaseB, currentPhaseB, Frequency);

  //Publish to the topic on AWS every 60 second
  if (millis() - lastPublish > 60000) {
      //String msg = String("Hello from ESP32: ") + ++msgCount;
      // boolean rc = pubSubClient.publish("outTopic", msg.c_str());
      boolean rc = pubSubClient.publish("/mqtt/test", energyMeterData);
      Serial.print("Published, rc="); Serial.print( (rc ? "OK: " : "FAILED: ") );
      Serial.println(energyMeterData);
      entryNo+=1;
      lastPublish = millis();
     } 

} // -- END of main loop

