#include <LiquidCrystal.h>
#define ECHO_PIN 3
#define TRIGGER_PIN 4

#define LCD_RS_PIN A5
#define LCD_E_PIN A4
#define LCD_D4_PIN 6
#define LCD_D5_PIN 7
#define LCD_D6_PIN 8
#define LCD_D7_PIN 9

LiquidCrystal lcd(
  LCD_RS_PIN,
  LCD_E_PIN,
  LCD_D4_PIN,
  LCD_D5_PIN,
  LCD_D6_PIN,
  LCD_D7_PIN
);

bool isDetected = false;

unsigned long lastUltrasonicTrigger = millis();
unsigned long ultrasonicTriggerDelay = 100;

volatile unsigned long pulseInTimeBegin;
volatile unsigned long pulseInTimeEnd;
volatile bool newDistanceAvailable = false;

double getUltrasonicDistance() {
  double durationMicros = pulseInTimeEnd- pulseInTimeBegin; 
  double distance = durationMicros/58.0; //cm
  return distance;
}

void triggerUltrasonicSensor() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
}

void echoPinInterrupt() {
  if(digitalRead(ECHO_PIN) == HIGH) { // start measuring
    // when signal is rising
    pulseInTimeBegin = micros();
  }
  else {
    // when signal is falling
    pulseInTimeEnd = micros();
    newDistanceAvailable = true;
  }
}

void clearRow(int row) {
  lcd.setCursor(0, row);         // Move cursor to start of the row
  for (int i = 0; i < 16; i++) { // Overwrite all columns with spaces
    lcd.print(" ");
  }
  lcd.setCursor(0, row);         // Optional: move cursor back to start
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino...");

  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(ECHO_PIN), echoPinInterrupt, CHANGE); // when its rising: low -> high

  lcd.begin(16, 2); // (columns, rows)
}

void loop() {
  // Serial.println("Hello Boss"); // message sent to esp32
  
  unsigned long timeNow = millis();

  if(timeNow - lastUltrasonicTrigger > ultrasonicTriggerDelay) {
    lastUltrasonicTrigger += ultrasonicTriggerDelay;
    triggerUltrasonicSensor();
  }

  if(newDistanceAvailable) {
    newDistanceAvailable = false;
    double distance = getUltrasonicDistance();

    if(distance <= 35) { //cm
      isDetected = true;
    }
    else {
      isDetected = false;
    }

    lcd.setCursor(0, 0);
    lcd.print("Distance: ");
    lcd.print(distance);
  }

  if(isDetected) {
    lcd.setCursor(0, 1);
    lcd.print("Person detected!");
  }
  else {
    clearRow(1);
  }
  // delay(1500);
}
