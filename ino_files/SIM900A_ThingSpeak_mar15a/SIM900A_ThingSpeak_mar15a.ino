//Define pins for UART2
#define RX2 16 
#define TX2 17

String S= "GET http://api.thingspeak.com/update?api_key=8TZPFLQB5W67R5NE&field1="+String(24);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RX2, TX2);
  Serial.println("UART2 Txd is on pin: 17");
  Serial.println("UART2 Rxd is on pin: 16");
  Serial.println("UART2 Ready"); 
}

void loop() {
  // put your main code here, to run repeatedly:
    Serial2.print("AT");
    Serial.println("AT");
    delay(2000);
    
    Serial2.print("AT+CPIN");
    Serial.println("AT+CPIN");
    delay(2000);
    
    Serial2.print("AT+CREG?");
    Serial.println("AT+CREG?");
    delay(4000);
    
    Serial2.print("AT+CGATT?");
    Serial.println("AT+CGATT?");
    delay(4000);

    Serial2.print("AT+CIPSHUT");
    Serial.println("AT+CIPSHUT");
    delay(4000);
    
    Serial2.print("AT+CIPSTATUS");
    Serial.println("AT+CIPSTATUS");
    delay(4000);
    
    Serial2.print("AT+CIPMUX=0\\r\\n");
    Serial.println("AT+CIPMUX=0");
    delay(4000);

    Serial2.print("AT+CSTT=\"airtelgprs.com\"\\r\\n");
    Serial.println("AT+CSTT=airtelgprs.com");
    delay(4000);

    Serial2.print("AT+CIICR\\r\\n");
    Serial.println("AT+CIICR");
    delay(4000);

    Serial2.print("AT+CIFSR\\r\\n");
    Serial.println("AT+CIFSR");
    delay(4000);

    Serial2.print("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"\\r\\n");
    Serial.println("AT+CIPSTART=TCP,api.thingspeak.com,80");
    delay(4000);

    Serial2.print("AT+CIPSEND\\r\\n");
    Serial.println("AT+CIPSEND");
    delay(4000);

    Serial2.print("GET http://api.thingspeak.com/update?api_key=8TZPFLQB5W67R5NE&field1=24\\r\\n");
    Serial.println("GET http://api.thingspeak.com/update?api_key=8TZPFLQB5W67R5NE&field1=24");
    delay(2000);

    Serial2.print((char)26);
    Serial.println("#026");
    delay(4000);   
}
