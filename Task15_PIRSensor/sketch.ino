/* Task 15: Internet-enabled PIR Sensor
 * ESP32 + PIR motion sensor -> LED indicator + Firebase Realtime Database.
 */
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASS = "";
String FIREBASE_HOST = "elearning-1408b-default-rtdb.asia-southeast1.firebasedatabase.app";
String FIREBASE_AUTH = "BV9noeROfWdPpnZ3oI6HAmHbb4kermXQqt8qYniG";

#define PIR 13
#define LED 2

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
  pinMode(PIR, INPUT); pinMode(LED, OUTPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(300); Serial.print("."); }
  Serial.println("\nWiFi connected");
}

void loop() {
  bool motion = digitalRead(PIR);
  digitalWrite(LED, motion);
  Serial.printf("Motion: %s\n", motion ? "DETECTED" : "none");
  String json = "{\"motion\":\"" + String(motion ? "DETECTED" : "NONE") + "\"}";
  fbPatch("/mciot_lab/task15_pir", json);
  delay(1500);
}
