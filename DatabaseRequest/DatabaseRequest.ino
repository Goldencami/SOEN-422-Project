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

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("FirebaseClient playground test");
  connectWifi();

  Serial.println("Enter UID into Serial Monitor");
}

void loop() {
  // put your main code here, to run repeatedly:

}