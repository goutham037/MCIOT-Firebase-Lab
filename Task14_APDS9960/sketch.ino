/* Task 14: Internet-enabled APDS-9960 (ambient light, proximity, gesture)
 * ESP32 + APDS-9960 -> Firebase Realtime Database.
 * Wokwi has no APDS-9960, so two potentiometers simulate ambient-light and
 * proximity, and a button simulates a detected gesture. On real hardware,
 * use the SparkFun_APDS9960 library on I2C (SDA=21, SCL=22) instead.
 */
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASS = "";
String FIREBASE_HOST = "elearning-1408b-default-rtdb.asia-southeast1.firebasedatabase.app";
String FIREBASE_AUTH = "BV9noeROfWdPpnZ3oI6HAmHbb4kermXQqt8qYniG";

#define LIGHT_PIN 34
#define PROX_PIN 35
#define GESTURE_BTN 14

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
  pinMode(GESTURE_BTN, INPUT_PULLUP);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(300); Serial.print("."); }
  Serial.println("\nWiFi connected");
}

void loop() {
  int light = map(analogRead(LIGHT_PIN), 0, 4095, 0, 1000);  // lux proxy
  int prox = map(analogRead(PROX_PIN), 0, 4095, 0, 255);      // proximity 0..255
  String gesture = (digitalRead(GESTURE_BTN) == LOW) ? "SWIPE" : "NONE";

  Serial.printf("Light=%d lux  Proximity=%d  Gesture=%s\n", light, prox, gesture.c_str());
  String json = "{\"ambient_light\":" + String(light) + ",\"proximity\":" + String(prox) +
                ",\"gesture\":\"" + gesture + "\"}";
  fbPatch("/mciot_lab/task14_apds9960", json);
  delay(1000);
}
