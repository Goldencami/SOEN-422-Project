#include <ESP32Servo.h>
#define TX_PIN 4
#define RX_PIN 13
#define SERVO_PIN 21
#define BUZZER_PIN 22
#define GREEN_LED_PIN 12
#define RED_LED_PIN 14
#define BTN_PIN 36

String correctCode = "test1234";

unsigned int debounceDelay = 50;
unsigned long lastBtnDebounce = millis();
byte btnState = LOW;

unsigned long servoDelay = 20; // change servo position every 20ms
unsigned long servoLastDelay = 0;

Servo myServo; // create servo object to control a servo
int pos = 0; // variable to store the servo position
bool isUnlocking = false;
bool isLocking = false;

bool alarmOn = false;

unsigned long beepInterval = 500;
unsigned long lastBeepTime = 0;
bool buzzerState = false;

unsigned long alarmDuration = 10000;
unsigned long alarmStartTime = 0;

byte redLEDState = LOW;
unsigned long redLEDBlink = 700;
unsigned long lastRedLEDBlink = millis();

// servo will rotate from 0 to 180 degrees
void unlockDoor() {
  unsigned long timeNow = millis();
  if ((timeNow - servoLastDelay > servoDelay) && pos < 180) {
    myServo.write(pos);
    // Serial.println("Unlocking: pos = " + String(pos));
    ++pos;
    servoLastDelay = timeNow;
  }
}

// servo will rotate from 180 to 0 degrees
void lockDoor() {
  unsigned long timeNow = millis();
  if ((timeNow - servoLastDelay > servoDelay) && pos > 0) {
    myServo.write(pos);
    // Serial.println("Locking: pos = " + String(pos));
    --pos;
    servoLastDelay = timeNow;
  }
}

void bipBuzzer() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_PIN, LOW);
  delay(500);
}

void handleFireAlarm() {
  byte newBtnState = digitalRead(BTN_PIN);

  // Start alarm if button pressed and alarm is not already on
  if(newBtnState == HIGH && !alarmOn) {
    alarmOn = true;
    alarmStartTime = millis(); // start the timer
    Serial.println("ALARM ACTIVATED!");
  }

  if(alarmOn) {
    unlockDoor();
    // Toggle buzzer ON/OFF every beepInterval
    unsigned long now = millis();
    if(now - lastBeepTime >= beepInterval) {
      buzzerState = !buzzerState;
      digitalWrite(BUZZER_PIN, buzzerState ? HIGH : LOW);
      blinkRedLED();
      lastBeepTime = now;
    }

    // Check if alarm duration has passed
    if(now - alarmStartTime >= alarmDuration) {
      alarmOn = false;
      digitalWrite(BUZZER_PIN, LOW); // ensure buzzer off
      buzzerState = false;
      Serial.println("Alarm ended.");
      lockDoor();
    }
  }
}

void blinkRedLED() {
  digitalWrite(GREEN_LED_PIN, LOW);
  unsigned long timeNow = millis();

  if(timeNow - lastRedLEDBlink > redLEDBlink) {
    if(redLEDState == LOW) {
      redLEDState = HIGH;
    }
    else {
      redLEDState = LOW;
    }

    digitalWrite(RED_LED_PIN, redLEDState);
    lastRedLEDBlink = timeNow;
  }
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);

  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  pinMode(BTN_PIN, INPUT);

  myServo.attach(SERVO_PIN); // attaches the servo to pin
  myServo.write(0);
}

void loop() {
  handleFireAlarm();
  if(alarmOn) {
    return; // block everything else while alarm is active
  }
  // digitalWrite(BUZZER_PIN, LOW);
  
  if (Serial2.available()) { // Only read if data is present
    String data = Serial2.readStringUntil('\n'); // Read full message
    data.trim(); // remove any trailing newline or spaces

    Serial.print("Message Received: ");
    Serial.println(data);

    if(correctCode == data) {
      isUnlocking = true;
      isLocking = false;
    }
    else {
      isUnlocking = false;
      isLocking = true;
      digitalWrite(GREEN_LED_PIN, LOW);
      digitalWrite(RED_LED_PIN, HIGH);
      bipBuzzer();
    }
    delay(700);
  }

  // handle unlocking
  if (isUnlocking) {
    unlockDoor();
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(RED_LED_PIN, LOW);

    if (pos >= 180) {
      Serial.println("Door fully unlocked");
      isUnlocking = false;
      isLocking = true;
      servoLastDelay = millis();
    }
  }

  // handle locking
  if (isLocking) {
    lockDoor();
    digitalWrite(GREEN_LED_PIN, LOW);

    if (pos <= 0) {
      Serial.println("Door fully locked");
      isLocking = false;
      myServo.detach();
      delay(500);
      myServo.attach(SERVO_PIN); // reattach for next cycle
    }
  }

  digitalWrite(RED_LED_PIN, LOW);
}
