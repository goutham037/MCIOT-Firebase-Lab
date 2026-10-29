/* Task 12: Internet-enabled Display of Ambient Parameters
 * ESP32 + DHT22 (temp/humidity) -> I2C LCD + Firebase Realtime Database.
 */
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASS = "";
String FIREBASE_HOST = "elearning-1408b-default-rtdb.asia-southeast1.firebasedatabase.app";
String FIREBASE_AUTH = "BV9noeROfWdPpnZ3oI6HAmHbb4kermXQqt8qYniG";

#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void fbPatch(String path, String json) {
  if (WiFi.status() != WL_CONNECTED) return;
  WiFiClientSecure c; c.setInsecure();
  HTTPClient h;
  h.begin(c, "https://" + FIREBASE_HOST + path + ".json?auth=" + FIREBASE_AUTH);
  h.addHeader("Content-Type", "application/json");
  int code = h.sendRequest("PATCH", json);
  Serial.printf("[Firebase] PATCH %s -> %d\n", path.c_str(), code);
  h.end();
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  lcd.init(); lcd.backlight();
  lcd.setCursor(0, 0); lcd.print("Ambient Monitor");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(300); Serial.print("."); }
  Serial.println("\nWiFi connected");
  delay(1500); lcd.clear();
}

void loop() {
  float t = dht.readTemperature();
  float hum = dht.readHumidity();
  if (isnan(t) || isnan(hum)) { Serial.println("DHT read failed"); delay(2000); return; }

  lcd.setCursor(0, 0); lcd.printf("Temp: %.1f C ", t);
  lcd.setCursor(0, 1); lcd.printf("Humidity: %.0f%% ", hum);
  Serial.printf("Temp=%.1fC  Humidity=%.0f%%\n", t, hum);

  String json = "{\"temperature\":" + String(t, 1) + ",\"humidity\":" + String(hum, 0) + "}";
  fbPatch("/mciot_lab/task12_ambient", json);
  delay(2000);
}
