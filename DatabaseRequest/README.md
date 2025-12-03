# DatabaseRequest

## Explaination of this section
This subdirectory is a playground to try out Firebases's Realtime Database with our ESP32. The [DatabaseRequest.ino](DatabaseRequest.ino). Is a simple trial of writing an UUID into the Serial Monitor and having it accepted or rejected by the Realtime Database. 

## Firebase data
- Created an `allowed_uids` node in the Realtime Database with keys set to normalized UIDs and boolean values indicating access (true = allowed).
- The sketch normalizes input by stripping whitespace and uppercasing (e.g., `ab cd ef` -> `ABCDEF`).

Example:
```json
{
  "allowed_uids": {
    "ABCDEFGH": true,
    "12345678": true
  }
}
```

## Setup
- Install the [FirebaseClient](https://github.com/mobizt/FirebaseClient) library (Arduino Library Manager).
- Copy `../secrets.example.h` to `DatabaseRequest/secrets.h`, then fill:
  - `WIFI_SSID` / `WIFI_PASSWORD`
  - `WEB_API_KEY` (Firebase Web API key)
  - `DATABASE_URL` (Realtime Database URL with `https://` and `.firebaseio.com`)
  - `USER_EMAIL` / `USER_PASSWORD` (Email/Password auth must be enabled in Firebase)

## How to run
1. Open `DatabaseRequest/DatabaseRequest.ino` in Arduino IDE, select your ESP32 board/port.
2. Upload, then open Serial Monitor at 115200 baud.
3. Enter a UID when prompted; the sketch queries `/allowed_uids/{UID}` and prints `ACCESS GRANTED` or `ACCESS DENIED` based on the Firebase value.

## Notes
- Good tutorial that goes over useful concepts for this part: https://randomnerdtutorials.com/esp32-firebase-realtime-database/
- Uses: [FirebaseClient module](https://github.com/mobizt/FirebaseClient) by [mobizt](https://github.com/mobizt).