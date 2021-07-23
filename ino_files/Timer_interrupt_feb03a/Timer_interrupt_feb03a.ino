
hw_timer_t * timer =NULL;
int LED1 = 2;
volatile uint8_t LED_status =0;


void IRAM_ATTR onTimer()
  {
    Serial.println(String("ontimer()")+String(millis()));
    LED_status = 1- LED_status;
    digitalWrite(LED1, LED_status); //Turn ON and OFF the LED 
  }


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, LOW);
  Serial.println("Start timer");
  
  // ESP-32 TIMER SETTINGS
  
  timer=timerBegin(0,80,true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer,1000000,true);
  timerAlarmEnable(timer);
}

void loop() {
  // put your main code here, to run repeatedly:

}
