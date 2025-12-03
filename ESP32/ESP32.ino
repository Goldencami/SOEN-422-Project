#define ENABLE_USER_AUTH // Only enables user authentication 
#define ENABLE_DATABASE  // Only enables database access

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>
#include <ESP32Servo.h>
#define TX_PIN 4
#define RX_PIN 13
#define SERVO_PIN 21
#define BUZZER_PIN 22
#define GREEN_LED_PIN 12
#define RED_LED_PIN 14
#define BTN_PIN 36
#define MQ1_PIN 39

#define IR_PIN 34

#include "secrets.h"

bool personExiting = false;
bool personEntering = false;

unsigned long MQdelay = 100;
unsigned long lastMQdelay = 0;

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

bool fireAlarmJustEnded = false;
unsigned long fireAlarmEndTime = 0;

unsigned long beepInterval = 500;
unsigned long lastBeepTime = 0;
bool buzzerState = false;

unsigned long alarmDuration = 10000;
unsigned long alarmStartTime = 0;

byte redLEDState = LOW;
unsigned long redLEDBlink = 700;
unsigned long lastRedLEDBlink = millis();

// Firebase objects
UserAuth user_auth(WEB_API_KEY, USER_EMAIL, USER_PASSWORD);
FirebaseApp app;
WiFiClientSecure ssl_client;
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);
RealtimeDatabase db;

// UID state shared with Firebase callback
String lastUid;

// Flags for synchronous wrapper
bool uidAllowed = false;
bool uidCheckCompleted = false;

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
  unsigned long timeNow = millis();

  if((detectSmoke() || handleButtonAlarm()) && !alarmOn) {
    alarmOn = true;
    alarmStartTime = timeNow; // only set star timer once
    Serial.println("FIRE ALARM ACTIVATED!");
  }

  if(alarmOn) {
    unlockDoor();
    // Toggle buzzer ON/OFF every beepInterval
    if(timeNow - lastBeepTime >= beepInterval) {
      buzzerState = !buzzerState;
      digitalWrite(BUZZER_PIN, buzzerState ? HIGH : LOW); // beep buzzer
      blinkRedLED();
      lastBeepTime = timeNow;
    }

    // Check if alarm duration has passed
    if(timeNow - alarmStartTime >= alarmDuration) {
      alarmOn = false;
      digitalWrite(BUZZER_PIN, LOW); // ensure buzzer off
      buzzerState = false;
      Serial.println("Alarm ended.");
      
      fireAlarmJustEnded = true;
      fireAlarmEndTime = millis();

      // Trigger locking
      isUnlocking = false;   // stop unlocking
      isLocking = true;      // start gradual locking
    }
  }
}

bool handleButtonAlarm() {
  byte newBtnState = digitalRead(BTN_PIN);

  // Start alarm if button pressed and alarm is not already on
  if(newBtnState == HIGH) {
    return true;
  }

  return false;
}

bool detectSmoke() {
  unsigned long timeNow = millis();

  if (timeNow - lastMQdelay >= MQdelay) {
    lastMQdelay = timeNow;
    
    int smokeVal = analogRead(MQ1_PIN);  // read MQ sensor
    // Serial.print("Smoke sensor value: ");
    // Serial.println(smokeVal);

    if(smokeVal > 2500) { // test value for smoke detection
      Serial.print("Smoke sensor value: ");
      Serial.println(smokeVal);
      return true;
    }

    return false;
    // Serial.println(analogRead(MQ1_PIN));
  }
  
  return false;
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

// Normalize UID to be a shortened uppercase string (ex: AB CD EF --> ABCDEF)
String normalizeUid(const String &raw) {
  String out;
  for (size_t i = 0; i < raw.length(); i++) {
    char c = raw[i];
    if (c != ' ' && c != '\r' && c != '\n' && c != '\t') out += (char)toupper(c);
  }
  return out;
}

void connectWifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println(" connected");
}

// Callback function used for authentication and database result
void firebaseCallback(AsyncResult &result) {
  if (!result.isResult()) return;   // ignore non-result callbacks

  // Realisiticaly this shouldn't happen although important to log if does
  if (result.isError()) {
    Serial.print("Firebase error: ");
    Serial.println(result.error().message());
    uidAllowed = false;
    uidCheckCompleted = true;
    return;
  }

  // Likewise with previous if-statement, shouldn't happen although good to log
  if (!result.available()) {
    Serial.println("Firebase returned result without payload");
    uidAllowed = false;
    uidCheckCompleted = true;
    return;
  }

 // Verify this return was for our call to checkUid (from db.get call)
  if (result.uid() == "checkUid") {
    String payload = result.c_str();  // Expected return: true, null
    payload.trim();

    Serial.print("Firebase raw payload for ");
    Serial.print(lastUid);
    Serial.print(" = '");
    Serial.print(payload);
    Serial.println("'");

    uidAllowed = (payload == "true");  // your DB stores boolean true
    uidCheckCompleted = true;
  }
}

void setupFirebase() {
  ssl_client.setInsecure();
  initializeApp(aClient, app, getAuth(user_auth), firebaseCallback);
  app.getApp<RealtimeDatabase>(db);
  db.url(DATABASE_URL);
}

bool validateUidWithFirebase(const String &rawUid) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, denying by default");
    return false;
  }
  if (!app.ready()) {
    Serial.println("Firebase not ready, denying by default");
    return false;
  }

  lastUid = normalizeUid(rawUid);
  String path = "/allowed_uids/" + lastUid;

  uidCheckCompleted = false;
  uidAllowed = false;

  // Note: Request is asynchronous
  db.get(aClient, path, firebaseCallback, false, "checkUid");

  // Wait up to 3 seconds (timeout) for the callback to set uidCheckCompleted
  unsigned long start = millis();
  const unsigned long timeout = 3000;

  // While not completed and within timeout, wait
  while (!uidCheckCompleted && (millis() - start < timeout)) {
    app.loop();  // let Firebase process responses
    delay(10);   // avoid tight busy loop
  }

  if (!uidCheckCompleted) {
    Serial.println("Firebase UID check timed out, denying by default");
    return false;
  }

  return uidAllowed;
}


void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);

  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  pinMode(BTN_PIN, INPUT);

  connectWifi();
  setupFirebase();

  pinMode(IR_PIN, INPUT);

  myServo.attach(SERVO_PIN); // attaches the servo to pin
  myServo.write(0);
}

void loop() {
  app.loop();

  handleFireAlarm();
  if(alarmOn) {
    return; // block everything else while alarm is active
  }
  
  // Read data transmitted by UART from Arduino UNO
  if (Serial2.available()) { // Only read if data is present
    String data = Serial2.readStringUntil('\n'); // Read full message
    data.trim(); // remove any trailing newline or spaces

    Serial.print("Message Received: ");
    Serial.println(data);

    bool personAllowed = validateUidWithFirebase(data);

    if(personAllowed && !personExiting) {
      isUnlocking = true;
      isLocking = false;
      personEntering = true;
      personExiting = false;
    }
    else {
      isUnlocking = false;
      isLocking = true;
      personEntering = false;
      personExiting = false;
      digitalWrite(GREEN_LED_PIN, LOW);
      digitalWrite(RED_LED_PIN, HIGH);
      bipBuzzer();
    }
    delay(700);
  }

  // handle exiting sensor
  int irValue = analogRead(IR_PIN);
  if(irValue > 2500 && !personEntering && !isUnlocking && (!fireAlarmJustEnded || millis() - fireAlarmEndTime > 3000)) {
      Serial.println(irValue);
      Serial.println("Exiting");
      isUnlocking = true;
      personExiting = true;
      personEntering = false;
  }

  // Reset the flag after 3 seconds
  if(fireAlarmJustEnded && millis() - fireAlarmEndTime > 3000) {
      fireAlarmJustEnded = false;
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
    digitalWrite(RED_LED_PIN, LOW);

    if (pos <= 0) {
      Serial.println("Door fully locked");
      isLocking = false;
      personEntering = false;
      personExiting = false;
      myServo.detach();
      delay(500);
      myServo.attach(SERVO_PIN); // reattach for next cycle
    }
  }

  digitalWrite(RED_LED_PIN, LOW);
}
