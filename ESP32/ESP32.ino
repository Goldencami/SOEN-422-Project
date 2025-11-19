#define TX_PIN 4
#define RX_PIN 13
#define SERVO_PIN 19
#define BUZZER_PIN 21
#define GREEN_LED_PIN 12
#define RED_LED_PIN 22

String code = "test1234"

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);

  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
}

void loop() {
  if (Serial2.available()) { // Only read if data is present
    String msg = Serial2.readString(); // Read full message
    msg.trim(); // remove any trailing newline or spaces

    Serial.print("Message Received: ");
    Serial.println(msg);

    if (msg == "1") {
      digitalWrite(GREEN_LED_PIN, HIGH);
      digitalWrite(RED_LED_PIN, LOW);
    }
    else if (msg == "0") {
      digitalWrite(GREEN_LED_PIN, LOW);
      digitalWrite(RED_LED_PIN, HIGH);
    }
  }
}
