#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
//#include <MFRC522DriverI2C.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>
#define RX_PIN 2
#define TX_PIN 5

#define ECHO_PIN 3
#define TRIGGER_PIN 4

#define LCD_RS_PIN A5
#define LCD_E_PIN A4
#define LCD_D4_PIN 6
#define LCD_D5_PIN 7
#define LCD_D6_PIN 8
#define LCD_D7_PIN 9

SoftwareSerial espSerial(RX_PIN, TX_PIN);

MFRC522DriverPinSimple ss_pin(10); // Configurable, see typical pin layout above.

MFRC522DriverSPI driver{ss_pin}; // Create SPI driver.
//MFRC522DriverI2C driver{}; // Create I2C driver.
MFRC522 mfrc522{driver};  // Create MFRC522 instance.

LiquidCrystal lcd(
  LCD_RS_PIN,
  LCD_E_PIN,
  LCD_D4_PIN,
  LCD_D5_PIN,
  LCD_D6_PIN,
  LCD_D7_PIN
);

bool isDetected = false;
// String userInput = "";
// bool waitingForInput = false;
String uid = "";
bool isScanned = false;

unsigned long lastUltrasonicTrigger = millis();
unsigned long ultrasonicTriggerDelay = 100;

volatile unsigned long pulseInTimeBegin;
volatile unsigned long pulseInTimeEnd;
volatile bool newDistanceAvailable = false;

unsigned long UARTdelay = 1000;
unsigned long lastUARTdelay = millis();

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

String readCard() {
	// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  // Select one of the cards.
	if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
		return;
	}

	// Dump debug info about the card; PICC_HaltA() is automatically called.
  MFRC522Debug::PICC_DumpToSerial(mfrc522, Serial, &(mfrc522.uid));

  // get UID string
  String cardUID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    cardUID += String(mfrc522.uid.uidByte[i], HEX);
    if (i < mfrc522.uid.size - 1) {
      cardUID += " ";
    }
  }
  cardUID.toUpperCase();

  // Halt PICC
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  isScanned = true;

  return cardUID;
}

void setup() {
  Serial.begin(115200);
  espSerial.begin(115200);

  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(ECHO_PIN), echoPinInterrupt, CHANGE); // when its rising: low -> high

  lcd.begin(16, 2); // (columns, rows)

  Serial.begin(115200);  // Initialize serial communications with the PC for debugging.
  while (!Serial);     // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4).
  mfrc522.PCD_Init();  // Init MFRC522 board.
  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial);	// Show details of PCD - MFRC522 Card Reader details.
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop() {
  unsigned long timeNow = millis();

  if(timeNow - lastUltrasonicTrigger > ultrasonicTriggerDelay) {
    lastUltrasonicTrigger += ultrasonicTriggerDelay;
    triggerUltrasonicSensor();
  }

  if(newDistanceAvailable) {
    newDistanceAvailable = false;
    double distance = getUltrasonicDistance();

    if(distance <= 35) { // c,
      // if(!waitingForInput) {
      //   waitingForInput = true; // start waiting for input
      // }
      uid = readCard();
      isDetected = true;

    } else {
      isDetected = false;
      isScanned = false;
      // waitingForInput = false; // cancel waiting if person leaves
      uid = "";
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

  if(isScanned && uid != "") {
     Serial.print("Sent to ESP32: ");
    espSerial.println(uid);
    uid = "";
    isScanned = false;
  }

  // // Non-blocking read from Serial Monitor
  // while(Serial.available() > 0 && waitingForInput) {
  //   char c = Serial.read();       // read one character at a time
  //   if(c == '\n') {               // end of input
  //     espSerial.println(userInput); // send to ESP32
  //     Serial.print("Sent to ESP32: ");
  //     Serial.println(userInput);
  //     userInput = "";
  //     waitingForInput = false;
  //   } else {
  //     userInput += c;             // build the string
  //   }
  // }
}
