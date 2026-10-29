# Task 7 – Mobile App Simple Interface (Location + Text-to-Speech)

This task has two halves: the **cloud publisher** (`sketch.ino`, ESP32 → Firebase) and a
**mobile app** built in **MIT App Inventor** that reads the data and speaks it.

## a) Location Sensor (mobile app)
In MIT App Inventor (appinventor.mit.edu):
1. Add components: `LocationSensor1`, `FirebaseDB1`, `Button` (Get Location), `Label` (shows address).
2. Set `FirebaseDB1.FirebaseURL` and `FirebaseDB1.FirebaseToken` to your Realtime Database.
3. Blocks:
   - When **LocationSensor1.LocationChanged(latitude, longitude, altitude)** →
     `FirebaseDB1.StoreValue("task07_location/latitude", latitude)` and the same for longitude;
     set the Label to `LocationSensor1.CurrentAddress`.

## b) Text-to-Speech (mobile app)
1. Add components: `TextToSpeech1`, `Button` (Speak), and reuse `FirebaseDB1`.
2. Blocks:
   - When **Button_Speak.Click** → `FirebaseDB1.GetValue("task07_location/message", "")`.
   - When **FirebaseDB1.GotValue(tag, value)** → `TextToSpeech1.Speak(value)`.

So the phone reads the message stored in Firebase (written by the ESP32 `sketch.ino`
or by the LocationSensor) and speaks it aloud.

## ESP32 half (`sketch.ino`)
Publishes a demo latitude/longitude and a spoken message to
`/mciot_lab/task07_location`. Press the button in Wokwi to nudge the coordinates and
watch them update live in Firebase and in the app.

Fill `FIREBASE_HOST` and `FIREBASE_AUTH` at the top of `sketch.ino` (see main README).
