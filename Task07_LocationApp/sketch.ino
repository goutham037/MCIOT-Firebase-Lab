/* Task 7: Mobile App simple interface  (a) Location Sensor  (b) Text-to-Speech
 * This sketch is the ESP32/cloud half: it publishes a location (lat/long) to
 * Firebase. A mobile app built in MIT App Inventor then reads this location and
 * speaks it aloud using its TextToSpeech component (see README.md).
 *
 * A button nudges the demo coordinates so you can see live updates in Firebase.
 */
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASS = "";
String FIREBASE_HOST = "elearning-1408b-default-rtdb.asia-southeast1.firebasedatabase.app";
String FIREBASE_AUTH = "BV9noeROfWdPpnZ3oI6HAmHbb4kermXQqt8qYniG";

#define BTN 14
double lat = 17.4600;   // GRIET, Hyderabad (demo)
double lng = 78.3200;

void fbPatch(String path, String json) {
  if (WiFi.status() != WL_CONNECTED) return;
  WiFiClientSecure c; c.setInsecure(); HTTPClient h;
  h.begin(c, "https://" + FIREBASE_HOST + path + ".json?auth=" + FIREBASE_AUTH);
  h.addHeader("Content-Type", "application/json");
  int code = h.sendRequest("PATCH", json);
  Serial.printf("[Firebase] PATCH %s -> %d\n", path.c_str(), code); h.end();
}

void setup() {
  Serial.begin(115200);
  pinMode(BTN, INPUT_PULLUP);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(300); Serial.print("."); }
  Serial.println("\nWiFi connected");
}

void loop() {
  if (digitalRead(BTN) == LOW) { lat += 0.0005; lng += 0.0005; delay(200); }
  String msg = "You are near GRIET, Hyderabad";
  Serial.printf("Location: %.4f, %.4f\n", lat, lng);

  String json = "{\"latitude\":" + String(lat, 4) + ",\"longitude\":" + String(lng, 4) +
                ",\"message\":\"" + msg + "\"}";
  fbPatch("/mciot_lab/task07_location", json);
  delay(2000);
}
