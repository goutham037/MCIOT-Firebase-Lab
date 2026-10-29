/* Task 13: Internet-enabled Home Safety and Security System
 * ESP32 + PIR (intruder) + gas sensor (potentiometer proxy) + buzzer.
 * Raises an alarm and reports status to Firebase Realtime Database.
 */
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASS = "";
String FIREBASE_HOST = "elearning-1408b-default-rtdb.asia-southeast1.firebasedatabase.app";
String FIREBASE_AUTH = "BV9noeROfWdPpnZ3oI6HAmHbb4kermXQqt8qYniG";

#define PIR 13
#define GAS_PIN 34        // potentiometer = gas level proxy
#define BUZZER 2
#define GAS_LIMIT 60       // percent

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
  pinMode(PIR, INPUT); pinMode(BUZZER, OUTPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(300); Serial.print("."); }
  Serial.println("\nWiFi connected");
}

void loop() {
  bool intruder = digitalRead(PIR);
  int gas = map(analogRead(GAS_PIN), 0, 4095, 0, 100);
  bool gasLeak = gas > GAS_LIMIT;
  bool alarm = intruder || gasLeak;
  digitalWrite(BUZZER, alarm ? HIGH : LOW);

  Serial.printf("Intruder=%s  Gas=%d%%  Alarm=%s\n",
                intruder ? "YES" : "NO", gas, alarm ? "ON" : "OFF");

  String json = "{\"intruder\":\"" + String(intruder ? "YES" : "NO") +
                "\",\"gas_level\":" + String(gas) +
                ",\"alarm\":\"" + String(alarm ? "ON" : "OFF") + "\"}";
  fbPatch("/mciot_lab/task13_security", json);
  delay(1500);
}
