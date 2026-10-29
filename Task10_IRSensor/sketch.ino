/* Task 10: Internet-enabled IR Sensor
 * ESP32 + IR obstacle sensor -> LED + Firebase Realtime Database.
 * Wokwi has no IR obstacle part, so a push-button simulates the IR output
 * (pressed = object detected = IR LOW). On real hardware use an IR module on the same pin.
 */
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASS = "";
String FIREBASE_HOST = "elearning-1408b-default-rtdb.asia-southeast1.firebasedatabase.app";
String FIREBASE_AUTH = "BV9noeROfWdPpnZ3oI6HAmHbb4kermXQqt8qYniG";

#define IR_PIN 14      // IR module OUT (active LOW when object detected)
#define LED 2
int count = 0;
bool last = false;

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
  pinMode(IR_PIN, INPUT_PULLUP); pinMode(LED, OUTPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(300); Serial.print("."); }
  Serial.println("\nWiFi connected");
}

void loop() {
  bool detected = (digitalRead(IR_PIN) == LOW);  // active low
  digitalWrite(LED, detected);
  if (detected && !last) count++;                // count rising detections
  last = detected;

  Serial.printf("Object: %s | Count: %d\n", detected ? "DETECTED" : "clear", count);
  String json = "{\"object\":\"" + String(detected ? "DETECTED" : "CLEAR") +
                "\",\"count\":" + String(count) + "}";
  fbPatch("/mciot_lab/task10_ir", json);
  delay(800);
}
