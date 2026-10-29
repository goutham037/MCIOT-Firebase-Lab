/* Task 11: Internet-enabled Smart Garden Maintenance
 * ESP32 + soil-moisture (potentiometer) + DHT22 + water pump (LED/relay).
 * Uploads readings to Firebase and auto-waters when soil is dry.
 * Also reads /mciot_lab/task11_garden/pump_cmd from Firebase for remote control.
 */
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "DHT.h"

const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASS = "";
String FIREBASE_HOST = "YOUR_PROJECT-default-rtdb.firebaseio.com";
String FIREBASE_AUTH = "YOUR_DATABASE_SECRET";

#define SOIL_PIN 34     // potentiometer (analog soil moisture)
#define DHTPIN 15
#define PUMP 2          // LED = water pump / relay
#define DRY_THRESHOLD 40  // percent

DHT dht(DHTPIN, DHT22);

void fbPatch(String path, String json) {
  if (WiFi.status() != WL_CONNECTED) return;
  WiFiClientSecure c; c.setInsecure(); HTTPClient h;
  h.begin(c, "https://" + FIREBASE_HOST + path + ".json?auth=" + FIREBASE_AUTH);
  h.addHeader("Content-Type", "application/json");
  int code = h.sendRequest("PATCH", json);
  Serial.printf("[Firebase] PATCH %s -> %d\n", path.c_str(), code); h.end();
}
String fbGet(String path) {
  WiFiClientSecure c; c.setInsecure(); HTTPClient h;
  h.begin(c, "https://" + FIREBASE_HOST + path + ".json?auth=" + FIREBASE_AUTH);
  int code = h.GET(); String p = (code == 200) ? h.getString() : ""; h.end(); return p;
}

void setup() {
  Serial.begin(115200);
  pinMode(PUMP, OUTPUT); dht.begin();
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(300); Serial.print("."); }
  Serial.println("\nWiFi connected");
}

void loop() {
  int raw = analogRead(SOIL_PIN);              // 0..4095
  int moisture = map(raw, 0, 4095, 0, 100);    // percent
  float t = dht.readTemperature();

  // remote override from Firebase: "ON" forces pump on, "OFF" forces off, else auto
  String cmd = fbGet("/mciot_lab/task11_garden/pump_cmd"); cmd.replace("\"", "");
  bool pump;
  if (cmd == "ON") pump = true;
  else if (cmd == "OFF") pump = false;
  else pump = (moisture < DRY_THRESHOLD);       // auto-water when dry

  digitalWrite(PUMP, pump ? HIGH : LOW);
  Serial.printf("Moisture=%d%%  Temp=%.1fC  Pump=%s (cmd=%s)\n",
                moisture, t, pump ? "ON" : "OFF", cmd.c_str());

  String json = "{\"moisture\":" + String(moisture) + ",\"temperature\":" + String(t, 1) +
                ",\"pump\":\"" + (pump ? "ON" : "OFF") + "\"}";
  fbPatch("/mciot_lab/task11_garden", json);
  delay(2000);
}
