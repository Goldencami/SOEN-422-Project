#include <ESP32Servo.h>
#define TX_PIN 4
#define RX_PIN 13
#define SERVO_PIN 19
#define BUZZER_PIN 21
#define GREEN_LED_PIN 12
#define RED_LED_PIN 22

String correctCode = "test1234";

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);

  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
}

void loop() {
  if (Serial2.available()) { // Only read if data is present
    String data = Serial2.readStringUntil('\n'); // Read full message
    data.trim(); // remove any trailing newline or spaces

    Serial.print("Message Received: ");
    Serial.println(data);

    if(correctCode == data) {
      digitalWrite(GREEN_LED_PIN, HIGH);
      digitalWrite(RED_LED_PIN, LOW);
    }
    else {
      digitalWrite(GREEN_LED_PIN, LOW);
      digitalWrite(RED_LED_PIN, HIGH);
    }
    delay(500);
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, LOW);
  }
}
