#include <LiquidCrystal_I2C.h>
#include<WiFi.h>
#include<NTPClient.h>
#include<WiFiUdp.h>

//WiFi Credintials
const char* ssid = "TP-LINK_39FA";                  // Your wifi Name       
//const char* ssid = "Galaxy A502CBF";
const char* password = "Qwertyuiop";          // Your wifi Password

//Define LED
#define Led_OnBoard 2

//Define IST  (Indian Standard Time) offset & string
const int utcOffsetInSeconds = 19800;
String formattedDate, formattedTime, dayStamp, timeStamp;

// set the LCD number of columns and rows
int lcdColumns = 20;
int lcdRows = 4;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  
String messageToScroll = "Shaival From National Infotech 2021";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void wifiSetup(){  
    Serial.print("Connecting to "); Serial.print(ssid);
    WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
    delay(1000);
    WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
    WiFi.begin(ssid, password);     //Connect to your WiFi router
    
    while (WiFi.status() != WL_CONNECTED) {
    lcd.setCursor(0, 0);
    lcd.print("Connecting to WiFi.."); 
    digitalWrite(Led_OnBoard, HIGH);
    delay(250);
    Serial.print(".");
    digitalWrite(Led_OnBoard, LOW);
    delay(250);
    }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Connected");
    delay(250);
    lcd.clear();
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());    
  }

void setup(){
  Serial.begin(115200);
  pinMode(Led_OnBoard, OUTPUT); 
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
  
  wifiSetup();
  digitalWrite(Led_OnBoard, LOW);
  timeClient.begin();
  

}

void loop(){

  timeClient.update();
  getCurrentTime();
 
  // set cursor to first column, first row
  lcd.setCursor(0, 0);
  lcd.print("Date: ");  
   
  lcd.setCursor(0, 1);
  lcd.print("Time: ");

  lcd.setCursor(7,0);
  lcd.print(dayStamp);
  
  lcd.setCursor(7,1);
  lcd.print(formattedTime); 
  delay(1000);   
}


void getCurrentTime(){
  formattedDate = timeClient.getFormattedDate();
  formattedTime = timeClient.getFormattedTime();
 
  // Extract date
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
//  Serial.print("DATE: ");
//  Serial.println(dayStamp);
  
  // Extract time
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
//  Serial.print("HOUR: ");
//  Serial.println(timeStamp);
  }


/* Function to scroll text
 * The function acepts the following arguments:
 * row: row number where the text will be displayed
 * message: message to scroll
 * delayTime: delay between each character shifting
 * lcdColumns: number of columns of your LCD
 */

void scrollText(int row, String message, int delayTime, int lcdColumns) {
  for (int i=0; i < lcdColumns; i++) {
    message = " " + message; 
  } 
  message = message + " "; 
  for (int pos = 0; pos < message.length(); pos++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(pos, pos + lcdColumns));
    delay(delayTime);
  }
}
