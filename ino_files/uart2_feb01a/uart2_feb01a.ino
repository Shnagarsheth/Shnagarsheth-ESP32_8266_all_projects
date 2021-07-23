

int LED1 = 2;
char data[255];
#define RX2 16 
#define TX2 17

void setup() {
  // put your setup code here, to run once:
//setup Serial 2 for communication with STM32 
  pinMode(LED1, OUTPUT);
  Serial.begin(57600);
  
  //Serial2.begin(baudrate,protocol,RX2,TX2);
  Serial2.begin(57600, SERIAL_8N1, RX2, TX2);
  
  Serial.println("UART2 Txd is on pin: 17");
  Serial.println("UART2 Rxd is on pin: 16");
  Serial.println("UART2 Ready");
}

void loop() {
  // put your main code here, to run repeatedly:
while (Serial2.available()) 
  {
    digitalWrite(LED1, HIGH);
    Serial2.readBytes(data,30);
    //Serial.println(char(Serial2.read()));
   Serial.print(data);
    
    delay(500);
//    for(int i=0;i<100;i++)
//      {
//       data[i]=i;
//       delay(10);
//       //Serial.println(data[i]);     
//      }
//    
//   for(int i=0;i<255;i++)
//     {
//      data[i]=char(Serial2.readString());
//      delay(10);
//      }
  }
   
  }
