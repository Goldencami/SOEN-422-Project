#include <SoftwareSerial.h>
#define RX_PIN 2
#define TX_PIN 3

SoftwareSerial mySerial(RX_PIN, TX_PIN);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  mySerial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  mySerial.println("Hello Boss");
  Serial.println("Message sent");
  delay(1500);
}
