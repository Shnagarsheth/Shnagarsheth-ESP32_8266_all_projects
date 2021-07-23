
int ledPin = 2;
int w=0;
void setup() {
  // put your setup code here, to run once:
// Set LED as output
    pinMode(ledPin, OUTPUT);
    
    // Serial monitor setup
    Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:

    w++;
    Serial.println("Hello25");
    digitalWrite(ledPin, HIGH);
    Serial.println(w);
    
    delay(500);
    
    Serial.println("world!");
    digitalWrite(ledPin, LOW);
    
    delay(500);
}
