String InString="";
bool stringcomplete=false;
#define RX2 16 
#define TX2 17

int LED1=2;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2);
  Serial.println("UART2 Txd is on pin: 17");
  Serial.println("UART2 Rxd is on pin: 16");
  Serial.println("UART2 Ready");
  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, LOW);
  InString.reserve(200);
}

void loop() {
  // put your main code here, to run repeatedly:
  serialEvent2();
  if(stringcomplete)
    {
      Serial.println(InString);
      InString="";
      stringcomplete=false;     
    }
}

void serialEvent2()
{
  if(Serial2.available())
  {
    digitalWrite(LED1,HIGH);
    char InChar=(char)Serial2.read();
    InString+=InChar;
    if(InChar=='\n')
    {
      stringcomplete=true;
    }
  }
}
