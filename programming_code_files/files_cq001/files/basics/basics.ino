#include <WiFi.h>
#include <WebServer.h>
#include "DFRobot_HumanDetection.h"

// Radar object on Serial1
DFRobot_HumanDetection hu(&Serial1);
// WiFi credentials for AP mode
const char* ssid = "FallDetector_AP";
const char* password = "12345678";

// ESP32 Web Server on port 80
WebServer server(80);

// Global variables for radar data
uint16_t presence = 0, motion = 0, bodyMove = 0, fallState = 0;

void setup() {
  // Serial connections
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 9, 10);  // RX, TX



Serial.println("Start initialization");
  while (hu.begin() != 0) {
    Serial.println("init error!!!");
    delay(1000);
  }
  Serial.println("Initialization successful");

  Serial.println("Start switching work mode");
  while (hu.configWorkMode(hu.eFallingMode) != 0) {
    Serial.println("error!!!");
    delay(1000);
  }
  Serial.println("Work mode switch successful");

  hu.configLEDLight(hu.eFALLLed, 1);         // Set HP LED switch, it will not light up even if the sensor detects a person present when set to 0.
  hu.configLEDLight(hu.eHPLed, 1);           // Set FALL LED switch, it will not light up even if the sensor detects a person falling when set to 0.
  hu.dmInstallHeight(300);                   // Set installation height, it needs to be set according to the actual height of the surface from the sensor, unit: CM.
  hu.dmFallTime(2);                          // Set fall time, the sensor needs to delay the current set time after detecting a person falling before outputting the detected fall, this can avoid false triggering, unit: seconds.
  hu.dmUnmannedTime(1);                      // Set unattended time, when a person leaves the sensor detection range, the sensor delays a period of time before outputting a no person status, unit: seconds.
  hu.dmFallConfig(hu.eResidenceTime, 20);   // Set dwell time, when a person remains still within the sensor detection range for more than the set time, the sensor outputs a stationary dwell status. Unit: seconds.
  hu.dmFallConfig(hu.eFallSensitivityC, 3);  // Set fall sensitivity, range 0~3, the larger the value, the more sensitive.
  hu.sensorRet();                            // Module reset, must perform sensorRet after setting data, otherwise the sensor may not be usable.

  Serial.print("Current work mode:");
  switch (hu.getWorkMode()) {
    case 1:
      Serial.println("Fall detection mode");
      break;
    case 2:
      Serial.println("Sleep detection mode");
      break;
    default:
      Serial.println("Read error");
  }

  Serial.print("HP LED status:");
  switch (hu.getLEDLightState(hu.eHPLed)) {
    case 0:
      Serial.println("Off");
      break;
    case 1:
      Serial.println("On");
      break;
    default:
      Serial.println("Read error");
  }
  Serial.print("FALL status:");
  switch (hu.getLEDLightState(hu.eFALLLed)) {
    case 0:
      Serial.println("Off");
      break;
    case 1:
      Serial.println("On");
      break;
    default:
      Serial.println("Read error");
  }

  Serial.printf("Radar installation height: %d cm\n", hu.dmGetInstallHeight());
  Serial.printf("Fall duration: %d seconds\n", hu.getFallTime());
  Serial.printf("Unattended duration: %d seconds\n", hu.getUnmannedTime());
  Serial.printf("Dwell duration: %d seconds\n", hu.getStaticResidencyTime());
  Serial.printf("Fall sensitivity: %d \n", hu.getFallData(hu.eFallSensitivity));
  Serial.println();
  Serial.println();

  

  // Setup WiFi Access Point
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Setup web server
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
//  Serial.print("Existing information:");
//  switch (hu.smHumanData(hu.eHumanPresence)) {
//    case 0:
//      Serial.println("No one is present");
//      break;
//    case 1:
//      Serial.println("Someone is present");
//      break;
//    default:
//      Serial.println("Read error");
//  }
//
//  Serial.print("Motion information:");
//  switch (hu.smHumanData(hu.eHumanMovement)) {
//    case 0:
//      Serial.println("None");
//      break;
//    case 1:
//      Serial.println("Still");
//      break;
//    case 2:
//      Serial.println("Active");
//      break;
//    default:
//      Serial.println("Read error");
//  }
//
//    Serial.printf("Body movement parameters:%d\n", hu.smHumanData(hu.eHumanMovingRange));
//  Serial.print("Fall status:");
//  switch (hu.getFallData(hu.eFallState)) {
//    case 0:
//      Serial.println("Not fallen");
//      break;
//    case 1:
//      Serial.println("Fallen");
//      break;
//    default:
//      Serial.println("Read error");
//  }
//
//  Serial.print("Stationary dwell status:");
//  switch (hu.getFallData(hu.estaticResidencyState)) {
//    case 0:
//      Serial.println("No stationary dwell");
//      break;
//    case 1:
//      Serial.println("Stationary dwell present");
//      break;
//    default:
//      Serial.println("Read error");
//  }
//  Serial.println();

  server.handleClient();
}

// Web page handler
void handleRoot() {
  String html = "<html><body>";
  
  // Add existing information about human presence
  html += "<h2>Existing Information:</h2>";
  switch (hu.smHumanData(hu.eHumanPresence)) {
    case 0:
      html += "<p>No one is present</p>";
      break;
    case 1:
      html += "<p>Someone is present</p>";
      break;
    default:
      html += "<p>Read error</p>";
  }

  // Add motion information
  html += "<h2>Motion Information:</h2>";
  switch (hu.smHumanData(hu.eHumanMovement)) {
    case 0:
      html += "<p>None</p>";
      break;
    case 1:
      html += "<p>Still</p>";
      break;
    case 2:
      html += "<p>Active</p>";
      break;
    default:
      html += "<p>Read error</p>";
  }

  // Add body movement parameters
  html += "<h2>Body Movement Parameters:</h2>";
  html += "<p>" + String(hu.smHumanData(hu.eHumanMovingRange)) + "</p>";

  // Add fall status
  html += "<h2>Fall Status:</h2>";
  switch (hu.getFallData(hu.eFallState)) {
    case 0:
      html += "<p>Not fallen</p>";
      break;
    case 1:
      html += "<p>Fallen</p>";
      break;
    default:
      html += "<p>Read error</p>";
  }

  // Add stationary dwell status
  html += "<h2>Stationary Dwell Status:</h2>";
  switch (hu.getFallData(hu.estaticResidencyState)) {
    case 0:
      html += "<p>No stationary dwell</p>";
      break;
    case 1:
      html += "<p>Stationary dwell present</p>";
      break;
    default:
      html += "<p>Read error</p>";
  }

//  // Add additional details if needed
//  html += "<h2>Additional Information:</h2>";
//  html += "<p>Radar Installation Height: " + String(hu.dmGetInstallHeight()) + " cm</p>";
//  html += "<p>Fall Duration: " + String(hu.getFallTime()) + " seconds</p>";
//  html += "<p>Unattended Duration: " + String(hu.getUnmannedTime()) + " seconds</p>";
//  html += "<p>Dwell Duration: " + String(hu.getStaticResidencyTime()) + " seconds</p>";
//  html += "<p>Fall Sensitivity: " + String(hu.getFallData(hu.eFallSensitivity)) + "</p>";
  
  // Close the HTML tags
  html += "</body></html>";

  // Send the HTML response to the client
  server.send(200, "text/html", html);
}
