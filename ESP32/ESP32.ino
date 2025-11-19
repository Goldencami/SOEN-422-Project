#define TX_PIN 4
#define RX_PIN 13

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
}

void loop() {
  if (Serial2.available()) { // Only read if data is present
    String msg = Serial2.readString(); // Read full message
    Serial.print("Message Received: ");
    Serial.println(msg);
  }
}
