/*
 * Project: Solar Panel Cleaner
 * Written by: Shaival Nagarsheth
 * Version: V1.0
 * Detailed Task: 1. Taking realTime from NTP server and making decision based on the actual time. 
 *                2. Set time for Non-functioning of the Cleaner, i.e. 7AM to 8PM (precisly, 6:57 to 19:59).
 *                3. If the start time entered is in between 20:01 to 6:56, then the cleaning pump will RUN for 3 minutes. 
 *                4. Log the time and date of the last 3 runs. 
 *                5. No webPage made in this version. 
 */

#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Ticker.h>

const char *ssid     = "TP-LINK_39FA";
const char *password = "Qwertyuiop"; 

const long utcOffsetInSeconds = 19800; //Offset in seconds for UTC: +5:30, which is IST (Indian Standard Time).

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

String currentDate;

//Define Timer
Ticker Timer1; //Declaring timer 1 for ISR

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// Assign output variables to GPIO pins
const int output1 = 16;
const int output2 = 2;

//Setting Time for Switching on the Cleaning Pump
int onHours = 11, onMinutes = 6;

//Declaring time and date variables
int currentHours = 0, currentMinutes=0 , currentSeconds=0,currentMonthDay =0, currentMonth = 0, currentYear =0;

//Declaring flag for PUMP ON/OFF status. 
uint8_t pumpOnFlag=0, printFlag =0;

void ISR1(){
  getCurrentTime();
  }

void wifiSetup(){
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
  Serial.print("Connected to -");  Serial.println(ssid);  Serial.print("IP address:");  Serial.println(WiFi.localIP());
  }

void setup(){
  Serial.begin(115200);
  
  //Initializing LEDs as Outputs
  pinMode(output1, OUTPUT); 
  pinMode(output2, OUTPUT); 
 
  // Set outputs to LOW
  digitalWrite(output1, HIGH);
  digitalWrite(output2, HIGH);
  
  Timer1.attach(10, ISR1); //ISR1 will be called every 10 seconds. //Use attach_ms if you need time in ms
  
  wifiSetup();
  timeClient.begin();
}

void loop() {
  timeClient.update(); //to get the current date and time from the NTP server.
  currentSeconds = timeClient.getSeconds();
  currentHours = timeClient.getHours();
  currentMinutes = timeClient.getMinutes();
  
  if ((currentHours >= 7) && (currentHours <= 20) && (pumpOnFlag==0)){
     if((onHours == currentHours) && (onMinutes == currentMinutes) && (currentSeconds == 0)){
        digitalWrite(output1,LOW);
        pumpOnFlag=1; printFlag = 1;             
      }    
    }

  if(printFlag == 1){
     printFlag =0;
     Serial.println("Relax! Your Solar Panels are being cleaned.");  
     }
      
  if(pumpOnFlag==1){    
    if (currentMinutes == (3 + onMinutes)){
      pumpOnFlag=0;
      digitalWrite(output1,HIGH);
      Serial.println("Your Solar Panels have been successfully cleaned.");  
      }
    }
     
}

void getCurrentTime(){  
  
  unsigned long epochTime = timeClient.getEpochTime();
   
  //Creating Time Structure 
  struct tm *ptm = gmtime((time_t *)&epochTime);
  currentMonthDay =ptm->tm_mday;
  currentMonth = ptm->tm_mon+1;
  currentYear = ptm->tm_year+1900; 
  currentDate = String(currentMonthDay) + "-" + String(currentMonth) + "-" + String(currentYear); 
  int cdate=currentDate.toInt();

  //Printing Date & Time to the Serial Monitor
  Serial.println(cdate);
  Serial.print("Date: ");
  Serial.print(currentDate);
  Serial.print(", ");
  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(". ");
  Serial.print("Time: ");
  Serial.print(currentHours);
  Serial.print(" : ");
  Serial.print(currentMinutes);
  Serial.print(" : ");
  Serial.println(currentSeconds);
  }
