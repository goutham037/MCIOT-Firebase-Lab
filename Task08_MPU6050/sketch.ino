/* Task 8: Sensor/Activator interfacing using ESP32 with MPU6050
 * Reads accelerometer + gyroscope and uploads motion data to Firebase.
 * An LED (activator) turns on when strong motion/tilt is detected.
 */
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASS = "";
String FIREBASE_HOST = "YOUR_PROJECT-default-rtdb.firebaseio.com";
String FIREBASE_AUTH = "YOUR_DATABASE_SECRET";

#define LED 2
Adafruit_MPU6050 mpu;

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
  pinMode(LED, OUTPUT);
  if (!mpu.begin()) { Serial.println("MPU6050 not found!"); while (1) delay(10); }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(300); Serial.print("."); }
  Serial.println("\nWiFi connected");
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float mag = sqrt(a.acceleration.x * a.acceleration.x +
                   a.acceleration.y * a.acceleration.y +
                   a.acceleration.z * a.acceleration.z);
  bool motion = fabs(mag - 9.8) > 3.0;     // deviation from gravity => motion/tilt
  digitalWrite(LED, motion ? HIGH : LOW);

  Serial.printf("Accel(%.2f,%.2f,%.2f) Gyro(%.2f,%.2f,%.2f) Motion=%s\n",
                a.acceleration.x, a.acceleration.y, a.acceleration.z,
                g.gyro.x, g.gyro.y, g.gyro.z, motion ? "YES" : "NO");

  String json = "{\"ax\":" + String(a.acceleration.x, 2) + ",\"ay\":" + String(a.acceleration.y, 2) +
                ",\"az\":" + String(a.acceleration.z, 2) + ",\"gx\":" + String(g.gyro.x, 2) +
                ",\"gy\":" + String(g.gyro.y, 2) + ",\"gz\":" + String(g.gyro.z, 2) +
                ",\"motion\":\"" + (motion ? "YES" : "NO") + "\"}";
  fbPatch("/mciot_lab/task08_mpu6050", json);
  delay(1500);
}
