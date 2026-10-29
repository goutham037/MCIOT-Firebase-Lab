/* Task 9: Internet-enabled Remote Range Indicator
 * ESP32 + HC-SR04 ultrasonic sensor -> Firebase Realtime Database.
 * Shows near/mid/far on 3 LEDs and uploads distance + status to the cloud.
 */
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASS = "";
// ===== Firebase Realtime Database config =====
String FIREBASE_HOST = "YOUR_PROJECT-default-rtdb.firebaseio.com";
String FIREBASE_AUTH = "YOUR_DATABASE_SECRET";

#define TRIG 5
#define ECHO 18
#define LED_GREEN 19
#define LED_YELLOW 21
#define LED_RED 22

void fbPatch(String path, String json) {
  if (WiFi.status() != WL_CONNECTED) return;
  WiFiClientSecure c; c.setInsecure();
  HTTPClient h;
  String url = "https://" + FIREBASE_HOST + path + ".json?auth=" + FIREBASE_AUTH;
  h.begin(c, url);
  h.addHeader("Content-Type", "application/json");
  int code = h.sendRequest("PATCH", json);
  Serial.printf("[Firebase] PATCH %s -> %d\n", path.c_str(), code);
  h.end();
}

float readDistanceCm() {
  digitalWrite(TRIG, LOW); delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long dur = pulseIn(ECHO, HIGH, 30000);
  return dur ? dur * 0.0343 / 2.0 : 400.0;
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG, OUTPUT); pinMode(ECHO, INPUT);
  pinMode(LED_GREEN, OUTPUT); pinMode(LED_YELLOW, OUTPUT); pinMode(LED_RED, OUTPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(300); Serial.print("."); }
  Serial.println("\nWiFi connected: " + WiFi.localIP().toString());
}

void loop() {
  float d = readDistanceCm();
  String status;
  digitalWrite(LED_GREEN, LOW); digitalWrite(LED_YELLOW, LOW); digitalWrite(LED_RED, LOW);
  if (d > 100)      { digitalWrite(LED_GREEN, HIGH);  status = "FAR"; }
  else if (d > 40)  { digitalWrite(LED_YELLOW, HIGH); status = "MID"; }
  else              { digitalWrite(LED_RED, HIGH);    status = "NEAR"; }

  Serial.printf("Distance: %.1f cm | Status: %s\n", d, status.c_str());
  String json = "{\"distance_cm\":" + String(d, 1) + ",\"status\":\"" + status + "\"}";
  fbPatch("/mciot_lab/task09_range", json);
  delay(2000);
}
