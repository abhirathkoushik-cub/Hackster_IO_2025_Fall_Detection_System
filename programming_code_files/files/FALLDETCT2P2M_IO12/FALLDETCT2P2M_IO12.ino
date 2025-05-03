#include <WiFi.h>
#include <WebServer.h>
#include "DFRobot_HumanDetection.h"

// Wi‑Fi AP credentials
const char* ssid     = "FallDetector_AP";
const char* password = "12345678";

// Web server on port 80
WebServer server(80);

// Radar object on Serial1
DFRobot_HumanDetection hu(&Serial1);

// Hardware IO pins for sensor outputs
#define FALL_PIN     12  // IO1: FALL status
#define PRESENCE_PIN 13  // IO2: PRESENCE status

// Forward declaration
void handleRoot();

void setup() {
  // --- Serial & Radar UART setup ---
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, /*RX=*/9, /*TX=*/10);

 Serial.println("Start initialization");
  while (hu.begin() != 0) {
    Serial.println("init error!!!");
    delay(1000);
  }
  Serial.println("Initialization successful");

  Serial.println("Switching to FALL mode");
  while (hu.configWorkMode(hu.eFallingMode) != 0) {
    Serial.println("mode switch error");
    delay(1000);
  }
  Serial.println("Fall mode active");

  // Sensor LED indicators
  hu.configLEDLight(hu.eFALLLed, 1); // FALL LED OFF when  fall
  hu.configLEDLight(hu.eHPLed,  1);  // PRESENCE LED ON when presence

  // Fall detection parameters
  hu.dmInstallHeight(220);              // mounting height: 220 cm
  hu.dmFallTime(2);                     // 2 s of static posture to trigger
  hu.dmUnmannedTime(1);                 // 1 s before marking "no person"
  hu.dmFallConfig(hu.eResidenceTime, 10);// 10 s of stillness → dwell alert
  hu.dmFallConfig(hu.eFallSensitivityC, 3); // max sensitivity

  // Apply settings
  hu.sensorRet();

  // Report current config
  Serial.printf("Mode: %d\n",    hu.getWorkMode());
  Serial.printf("Height: %d cm\n",hu.dmGetInstallHeight());
  Serial.printf("Fall Time: %d s\n", hu.getFallTime());
  Serial.printf("Unmanned Time: %d s\n", hu.getUnmannedTime());
  Serial.printf("Dwell Time: %d s\n", hu.getStaticResidencyTime());
  Serial.printf("Sensitivity: %d\n", hu.getFallData(hu.eFallSensitivity));
  Serial.println();



  // --- Hardware IO setup ---
  pinMode(FALL_PIN,     INPUT);
  pinMode(PRESENCE_PIN, INPUT);

  // --- Wi‑Fi AP & Web server setup ---
  WiFi.softAP(ssid, password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta charset='utf-8'>"
                "<title>FallDetector</title></head><body>";

  // ----- UART‐based sensor data -----
  html += "<h2>Presence (UART):</h2>";
  switch (hu.smHumanData(hu.eHumanPresence)) {
    case 0: html += "<p>No one present</p>"; break;
    case 1: html += "<p>Someone present</p>"; break;
    default: html += "<p>Read error</p>";
  }

  html += "<h2>Motion (UART):</h2>";
  switch (hu.smHumanData(hu.eHumanMovement)) {
    case 0: html += "<p>None</p>"; break;
    case 1: html += "<p>Still</p>"; break;
    case 2: html += "<p>Active</p>"; break;
    default: html += "<p>Read error</p>";
  }

  html += "<h2>Movement Range:</h2>"
          "<p>" + String(hu.smHumanData(hu.eHumanMovingRange)) + "</p>";

  html += "<h2>Fall State (UART):</h2>";
  switch (hu.getFallData(hu.eFallState)) {
    case 0: html += "<p>Not fallen</p>"; break;
    case 1: html += "<p>Fallen</p>"; break;
    default: html += "<p>Read error</p>";
  }

  html += "<h2>Static Dwell (UART):</h2>";
  switch (hu.getFallData(hu.estaticResidencyState)) {
    case 0: html += "<p>No dwell</p>"; break;
    case 1: html += "<p>Dwell present</p>"; break;
    default: html += "<p>Read error</p>";
  }

  // ----- Hardware IO pins -----
  html += "<h2>Fall Pin (IO1):</h2>";
  if (digitalRead(FALL_PIN) == HIGH) {
    html += "<p style='color:red;'>HIGH – FALL DETECTED</p>";
  } else {
    html += "<p>LOW – No Fall</p>";
  }
  
#include <WiFi.h>
#include <WebServer.h>
#include "DFRobot_HumanDetection.h"

// Wi‑Fi AP credentials
const char* ssid     = "FallDetector_AP";
const char* password = "12345678";

// Web server on port 80
WebServer server(80);

// Radar object on Serial1
DFRobot_HumanDetection hu(&Serial1);

// Hardware IO pins for sensor outputs
#define FALL_PIN     12  // IO1: FALL status
#define PRESENCE_PIN 13  // IO2: PRESENCE status

// Forward declaration
void handleRoot();

void setup() {
  // --- Serial & Radar UART setup ---
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, /*RX=*/9, /*TX=*/10);

  Serial.println("Start initialization");
  while (hu.begin() != 0) {
    Serial.println("init error!!!");
    delay(1000);
  }
  Serial.println("Initialization successful");

  Serial.println("Switching to FALL mode");
  while (hu.configWorkMode(hu.eFallingMode) != 0) {
    Serial.println("mode switch error");
    delay(1000);
  }
  Serial.println("Fall mode active");

  // Sensor LED indicators
  hu.configLEDLight(hu.eFALLLed, 1); // FALL LED OFF when  fall
  hu.configLEDLight(hu.eHPLed,  1);  // PRESENCE LED ON when presence

  // Fall detection parameters
  hu.dmInstallHeight(220);              // mounting height: 220 cm
  hu.dmFallTime(2);                     // 2 s of static posture to trigger
  hu.dmUnmannedTime(1);                 // 1 s before marking "no person"
  hu.dmFallConfig(hu.eResidenceTime, 10);// 10 s of stillness → dwell alert
  hu.dmFallConfig(hu.eFallSensitivityC, 3); // max sensitivity

  // Apply settings
  hu.sensorRet();

  // Report current config
  Serial.printf("Mode: %d\n",    hu.getWorkMode());
  Serial.printf("Height: %d cm\n",hu.dmGetInstallHeight());
  Serial.printf("Fall Time: %d s\n", hu.getFallTime());
  Serial.printf("Unmanned Time: %d s\n", hu.getUnmannedTime());
  Serial.printf("Dwell Time: %d s\n", hu.getStaticResidencyTime());
  Serial.printf("Sensitivity: %d\n", hu.getFallData(hu.eFallSensitivity));
  Serial.println();



  // --- Hardware IO setup ---
  pinMode(FALL_PIN,     INPUT);
  pinMode(PRESENCE_PIN, INPUT);

  // --- Wi‑Fi AP & Web server setup ---
  WiFi.softAP(ssid, password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta charset='utf-8'>"
                "<title>FallDetector</title></head><body>";

  // ----- UART‐based sensor data -----
  html += "<h2>Presence (UART):</h2>";
  switch (hu.smHumanData(hu.eHumanPresence)) {
    case 0: html += "<p>No one present</p>"; break;
    case 1: html += "<p>Someone present</p>"; break;
    default: html += "<p>Read error</p>";
  }

  html += "<h2>Motion (UART):</h2>";
  switch (hu.smHumanData(hu.eHumanMovement)) {
    case 0: html += "<p>None</p>"; break;
    case 1: html += "<p>Still</p>"; break;
    case 2: html += "<p>Active</p>"; break;
    default: html += "<p>Read error</p>";
  }

  html += "<h2>Movement Range:</h2>"
          "<p>" + String(hu.smHumanData(hu.eHumanMovingRange)) + "</p>";

  html += "<h2>Fall State (UART):</h2>";
  switch (hu.getFallData(hu.eFallState)) {
    case 0: html += "<p>Not fallen</p>"; break;
    case 1: html += "<p>Fallen</p>"; break;
    default: html += "<p>Read error</p>";
  }

  html += "<h2>Static Dwell (UART):</h2>";
  switch (hu.getFallData(hu.estaticResidencyState)) {
    case 0: html += "<p>No dwell</p>"; break;
    case 1: html += "<p>Dwell present</p>"; break;
    default: html += "<p>Read error</p>";
  }

  // ----- Hardware IO pins -----
  html += "<h2>Fall Pin (IO1):</h2>";
  if (digitalRead(FALL_PIN) == HIGH) {
    html += "<p style='color:red;'>HIGH – FALL DETECTED</p>";
  } else {
    html += "<p>LOW – No Fall</p>";
  }

  html += "<h2>Presence Pin (IO2):</h2>";
  if (digitalRead(PRESENCE_PIN) == HIGH) {
    html += "<p style='color:green;'>HIGH – PRESENCE DETECTED</p>";
  } else {
    html += "<p>LOW – No Presence</p>";
  }

  html += "</body></html>";
  server.send(200, "text/html", html);
}

  html += "<h2>Presence Pin (IO2):</h2>";
  if (digitalRead(PRESENCE_PIN) == HIGH) {
    html += "<p style='color:green;'>HIGH – PRESENCE DETECTED</p>";
  } else {
    html += "<p>LOW – No Presence</p>";
  }

  html += "</body></html>";
  server.send(200, "text/html", html);
}
