#define MQ1_PIN A0

unsigned long MQdelay = 400;
unsigned long lastMQdelay = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  unsigned long timeNow = millis();

  if (timeNow - lastMQdelay >= MQdelay) {
    lastMQdelay = timeNow;
    
    int gasValue = analogRead(MQ1_PIN);
    Serial.println(gasValue);
  }
}