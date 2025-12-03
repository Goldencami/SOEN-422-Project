// Enable only what we need
#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>

#include "secrets.h"

// Firebase objects and stuff
UserAuth user_auth(WEB_API_KEY, USER_EMAIL, USER_PASSWORD);
FirebaseApp app;
WiFiClientSecure ssl_client;
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);
RealtimeDatabase db;
String lastUid;

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

  if (result.isError()) {
    Serial.print("Firebase error: ");
    Serial.println(result.error().message());
    return;
  }

  if (!result.available()) {
    Serial.print("Firebase returned result without payload");
    Serial.println(result.error().message());
    return;
  }

  // Verify this return was for our call to checkUid (from db.get call)
  if (result.uid() == "checkUid") {
    String payload = result.c_str(); // Expected return: true, false, null, "" 
    payload.trim();
    Serial.print("Firebase raw payload for ");
    Serial.print(lastUid);
    Serial.print(" = '");
    Serial.print(payload);
    Serial.println("'");

    bool allowed = (payload == "true");
    if (allowed) {
      Serial.println("ACCESS GRANTED");
    } else {
      Serial.println("ACCESS DENIED");
    }
    Serial.println();
  }
}

void setupFirebase() {
  ssl_client.setInsecure();
  initializeApp(aClient, app, getAuth(user_auth), firebaseCallback); // client, app, authenticaiton and callback needed
  app.getApp<RealtimeDatabase>(db);
  db.url(DATABASE_URL);
}


void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("FirebaseClient playground test");
  connectWifi();
  setupFirebase();

  Serial.println("Enter UID into Serial Monitor");
}

void loop() {
  app.loop();  // important to keep app running

  if (Serial.available()) { // This is only for testing, we won't need input for actual project
    String raw = Serial.readStringUntil('\n'); // read input from Serial Monitor
    raw.trim();
    if (!raw.length()) return; // do nothing if user enters nothing lol

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi not connected");
      return;
    }
    if (!app.ready()) {
      Serial.println("Firebase not ready, trying again");
      return;
    }

    lastUid = normalizeUid(raw);
    String path = "/allowed_uids/" + lastUid;

    Serial.print("Checking UID ");
    Serial.print(lastUid);
    Serial.print(" at path: ");
    Serial.println(path);

    db.get(aClient, path, firebaseCallback, false, "checkUid");
  }
}
