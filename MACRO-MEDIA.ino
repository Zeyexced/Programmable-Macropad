#include <HID-Project.h>

// Define sensor pins
const int hallSensorPins[9] = { A0, A3, A8, A1, A6, A9, A2, A7, A10 };

// Macro commands for each sensor (Windows-specific key combinations)
const char* macros[9] = {
  "chrome",     // Chrome browser
  "zoom",    // Discord
  "excel",      // Excel
  "word",       // Word
  "snipping tool",      // Steam
  "calculator",       // Calculator
  "explorer",   // File Explorer
  "notepad",    // Notepad
  "powerpoint"     // Task Manager
};

// Threshold for detecting activation
const int activationThreshold = 400;

// Rotary encoder pins
const int clkPin = 2; // CLK pin of KY-040
const int dtPin = 3;  // DT pin of KY-040

// Variables for rotary encoder state
int lastClkState;
bool volumeChanged = false;

// Array to store the previous state of each sensor
bool keyState[9] = {false, false, false, false, false, false, false, false, false};

void setup() {
  Serial.begin(9600);

  // Set hall sensor pins as inputs
  for (int i = 0; i < 9; i++) {
    pinMode(hallSensorPins[i], INPUT);
  }

  // Set rotary encoder pins as inputs
  pinMode(clkPin, INPUT);
  pinMode(dtPin, INPUT);

  // Initialize the last state of CLK
  lastClkState = digitalRead(clkPin);

  Consumer.begin();
  Keyboard.begin();
}

void loop() {
  // Check hall effect sensors
  for (int i = 0; i < 9; i++) {
    int sensorValue = analogRead(hallSensorPins[i]);

    // Debugging: Print sensor values to Serial Monitor
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(" Value: ");
    Serial.println(sensorValue);

    // Check if the sensor is activated
    if (sensorValue < activationThreshold) {
      if (!keyState[i]) {
        launchMacro(macros[i]);
        keyState[i] = true;
      }
    } else {
      keyState[i] = false;
    }
  }

  // Check rotary encoder for volume control
  int clkState = digitalRead(clkPin);
  if (clkState != lastClkState) {
    int dtState = digitalRead(dtPin);
    if (clkState == LOW) {
      if (dtState != clkState) {
        Consumer.write(MEDIA_VOLUME_UP);
        Serial.println("Volume Up");
      } else {
        Consumer.write(MEDIA_VOLUME_DOWN);
        Serial.println("Volume Down");
      }
    }
    volumeChanged = true;
  }
  lastClkState = clkState;
}

// Function to launch macros using Windows Search
void launchMacro(const char* app) {
  Keyboard.press(KEY_LEFT_GUI); // Press Windows key
  delay(200);                   // Small delay to ensure it registers
  Keyboard.release(KEY_LEFT_GUI);
  delay(200);
  Keyboard.print(app);          // Type the app name
  delay(200);
  Keyboard.write(KEY_RETURN);   // Press Enter to launch
}
