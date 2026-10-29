# MCIOT Lab – Internet-Enabled (Firebase) Projects (Tasks 7–15)

ESP32 + Firebase Realtime Database projects for the **Microcontroller & IoT Lab (GR24A3095)**.
Every task pushes its sensor data to Firebase and (where relevant) reads control values
back. All sketches are written for the **Wokwi** online ESP32 simulator, which has real
internet access — so when you run them in Wokwi they genuinely write to your Firebase
database (that is the "execution").

## Tasks
| Folder | Task | Sensor / Component |
|--------|------|--------------------|
| Task07_LocationApp | Mobile app simple interface (location + TTS) | ESP32 publishes GPS-style location to Firebase |
| Task08_MPU6050 | Sensor/Activator interfacing with ESP32 | MPU6050 accelerometer + gyroscope |
| Task09_RangeIndicator | Internet-enabled remote range indicator | HC-SR04 ultrasonic + LEDs |
| Task10_IRSensor | Internet-enabled IR sensor | IR obstacle (button proxy in Wokwi) |
| Task11_SmartGarden | Internet-enabled smart garden maintenance | Soil moisture (pot) + DHT22 + pump (LED) |
| Task12_AmbientDisplay | Internet-enabled ambient parameter display | DHT22 + I2C LCD |
| Task13_HomeSecurity | Internet-enabled home safety & security | PIR motion + gas (pot) + buzzer |
| Task14_APDS9960 | Internet-enabled light/proximity/gesture | APDS-9960 (analog proxies in Wokwi) |
| Task15_PIRSensor | Internet-enabled PIR sensor | PIR motion + LED |

## How to run in Wokwi (execution)
1. Go to https://wokwi.com → **New Project → ESP32**.
2. Open the task folder here. Copy `sketch.ino` into Wokwi's code editor.
3. Click the **diagram.json** tab in Wokwi and paste this folder's `diagram.json`.
4. Add the required libraries in Wokwi (Library Manager): see each task's README
   (e.g. *Adafruit MPU6050*, *DHT sensor library*, *LiquidCrystal I2C*).
5. Put your Firebase details at the top of `sketch.ino` (see below) and press **Play**.
6. Open your Firebase console → Realtime Database to watch the data update live.

## Firebase configuration (fill these in every sketch)
```cpp
String FIREBASE_HOST = "YOUR_PROJECT-default-rtdb.firebaseio.com";  // no https://, no trailing /
String FIREBASE_AUTH = "YOUR_DATABASE_SECRET";                      // RTDB secret (or set rules to test mode)
```
- **Host:** Firebase Console → Realtime Database → the URL shown, without `https://` and the trailing `/`.
- **Secret:** Project Settings → Service accounts → Database secrets → *Show/Copy*.
  (Legacy secrets may need enabling; alternatively set the database **Rules** to test mode:
  `{ "rules": { ".read": true, ".write": true } }` for the lab.)

WiFi in Wokwi is always `Wokwi-GUEST` with an empty password (already set in the sketches).
