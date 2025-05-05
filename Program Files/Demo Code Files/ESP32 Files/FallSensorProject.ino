#include <WiFi.h>
#include <WebServer.h>
#include <ESP_Mail_Client.h>
#include "DFRobot_HumanDetection.h"

#define PIR_PIN 12 // GPIO pin connected to PIR sensor output


//// Replace with your WiFi credentials
const char *ssid = "wifi_name";// replace it with wifi name
const char *password = "PVPD515$"; // replace it with wifi password
// To send Emails using Gmail on port 465 (SSL), you need to create an app password: https://support.google.com/accounts/answer/185833
#define emailSenderAccount "fallsensoralert@gmail.com"
#define emailSenderPassword "asdfasdfasdfasdf" // replace it with gmail key
#define smtpServer "smtp.gmail.com"
#define smtpServerPort 465
#define emailSubject "Presence ALERT "

// Default Recipient Email Address
String inputMessage = "fallsensoralert@gmail.com";
String enableEmailChecked = "checked";
String inputMessage2 = "true";

// Flag variable to keep track if email notification was sent or not
bool emailSent = false;



// Access Point credentials
const char *ap_ssid = "ESP32-AP";
const char *ap_password = "12345678";
//
WebServer server(80);
// Radar object on Serial1
DFRobot_HumanDetection hu(&Serial1);
// SMTP session object
SMTPSession smtp;


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

//  // Add body movement parameters
//  html += "<h2>Body Movement Parameters:</h2>";
//  html += "<p>" + String(hu.smHumanData(hu.eHumanMovingRange)) + "</p>";

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

//  // Add stationary dwell status
//  html += "<h2>Stationary Dwell Status:</h2>";
//  switch (hu.getFallData(hu.estaticResidencyState)) {
//    case 0:
//      html += "<p>No stationary dwell</p>";
//      break;
//    case 1:
//      html += "<p>Stationary dwell present</p>";
//      break;
//    default:
//      html += "<p>Read error</p>";
//  }

  // Add PIR sensor status
  html += "<h2>PIR Sensor Status:</h2>";
  int pirValue = digitalRead(PIR_PIN);
  html += "<p>" + String(pirValue == HIGH ? "Motion Detected" : "No Motion") + "</p>";


  
  // Close the HTML tags
  html += "</body></html>";

  // Send the HTML response to the client
  server.send(200, "text/html", html);
}




void updateSerial()
{
  delay(500);
  while (Serial.available())
  {
    Serial2.write(Serial.read()); // Forward what Serial received to Software Serial Port
  }
  while (Serial2.available())
  {
    Serial.write(Serial2.read()); // Forward what Software Serial received to Serial Port
  }
}
bool waitForNetwork()
{
  String response = "";
  char c;
  while (true)
  {
    Serial2.println("AT+CREG?");
    delay(1000);

    response = "";
    while (Serial2.available())
    {
      c = Serial2.read();
      response += c;
    }
    Serial.print(response); // Print the module response to Serial monitor

    if (response.indexOf("+CREG: 0,1") != -1 || response.indexOf("+CREG: 0,5") != -1)
    {
      Serial.println("Network registered!");
      return true;
    }
    else
    {
      Serial.println("Waiting for network...");
      delay(2000);
    }
  }
}

void test_sim800_module()
{
  Serial2.println("AT");
  updateSerial();

  Serial2.println("AT+CMEE=2");
  updateSerial();

  Serial2.println("AT+CSQ");
  updateSerial();

  Serial2.println("AT+CCID");
  updateSerial();

  waitForNetwork(); // Wait until registered

  Serial2.println("ATI");
  updateSerial();

  Serial2.println("AT+CBC");
  updateSerial();
}

void send_SMS()
{
  Serial2.println("AT+CMGF=1"); // Set SMS to Text Mode
  updateSerial();

  Serial2.println("AT+CMGS=\"+13034722116\""); // Replace with your phone number
  delay(3000);                                 // Wait for '>' prompt
  updateSerial();

  Serial2.print("Circuit Digest"); // SMS body
  delay(500);
  Serial2.write(26); // ASCII code of CTRL+Z
  delay(1000);       // Wait for message to be sent
  Serial.println("Message Sent");
  updateSerial();
}

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
      Serial.println("WiFi Failed!");
    }
  Serial2.begin(9600, SERIAL_8N1, 9, 10);    // RX=16, TX=17 (or any other valid pins)
  Serial1.begin(115200, SERIAL_8N1, 13, 15); // RX, TX

  Serial.println("Start initialization");
  while (hu.begin() != 0)
  {
    Serial.println("init error!!!");
    delay(1000);
  }
  Serial.println("Initialization successful");

  Serial.println("\nConnected to WiFi");
  Serial.print("STA IP: ");
  Serial.println(WiFi.localIP());

  // Start Access Point
  WiFi.softAP(ap_ssid, ap_password);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  // Web server endpoints
  server.on("/", handleRoot);

  Serial.println("Web server started");
  pinMode(PIR_PIN, INPUT);
  pinMode(22, OUTPUT);
  digitalWrite(22, LOW); 

  //  delay(1000 * 20);
  Serial.println("Statrtchecking");



  Serial.println("Start switching work mode");
  while (hu.configWorkMode(hu.eFallingMode) != 0)
  {
    Serial.println("error!!!");
    delay(1000);
  }
  Serial.println("Work mode switch successful");

  hu.configLEDLight(hu.eFALLLed, 1);        // Set HP LED switch, it will not light up even if the sensor detects a person present when set to 0.
  hu.configLEDLight(hu.eHPLed, 1);          // Set FALL LED switch, it will not light up even if the sensor detects a person falling when set to 0.
  hu.dmInstallHeight(10);                  // Set installation height, it needs to be set according to the actual height of the surface from the sensor, unit: CM.
  hu.dmFallTime(2);                         // Set fall time, the sensor needs to delay the current set time after detecting a person falling before outputting the detected fall, this can avoid false triggering, unit: seconds.
  hu.dmUnmannedTime(1);                     // Set unattended time, when a person leaves the sensor detection range, the sensor delays a period of time before outputting a no person status, unit: seconds.
  hu.dmFallConfig(hu.eResidenceTime, 20);   // Set dwell time, when a person remains still within the sensor detection range for more than the set time, the sensor outputs a stationary dwell status. Unit: seconds.
  hu.dmFallConfig(hu.eFallSensitivityC, 3); // Set fall sensitivity, range 0~3, the larger the value, the more sensitive.
  hu.sensorRet();                           // Module reset, must perform sensorRet after setting data, otherwise the sensor may not be usable.
  Serial.printf("Radar installation height: %d cm\n", hu.dmGetInstallHeight());
  Serial.printf("Fall duration: %d seconds\n", hu.getFallTime());
  Serial.printf("Unattended duration: %d seconds\n", hu.getUnmannedTime());
  Serial.printf("Dwell duration: %d seconds\n", hu.getStaticResidencyTime());
  Serial.printf("Fall sensitivity: %d \n", hu.getFallData(hu.eFallSensitivity));
  Serial.println();
  Serial.println();
}

void loop()
{
  int statusofpir = digitalRead(PIR_PIN);
  int humansensor = hu.smHumanData(hu.eHumanPresence);
  Serial.printf("%d,%d",statusofpir,humansensor);
  if ((statusofpir == 1 ) and (humansensor==1))
  {
    digitalWrite(22, HIGH);
    sendEmailNotification("Pressence Detected");
    Serial.println("BLE Alert send");
    
    digitalWrite(22, LOW);
  }
  server.handleClient();
}
bool sendEmailNotification(String emailMessage)
{
    String html = "<html><head><style>"
                  "body { font-family: Arial, sans-serif; margin: 20px; padding: 0; background-color: #f4f4f4; }"
                  "h2 { color: #4CAF50; font-size: 20px; }"
                  "p { font-size: 16px; color: #333; line-height: 1.6; }"
                  ".container { background-color: #ffffff; padding: 20px; border-radius: 8px; box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1); max-width: 600px; margin: auto; }"
                  ".section { margin-bottom: 20px; }"
                  ".section p { background-color: #f9f9f9; padding: 10px; border-radius: 5px; }"
                  "</style></head><body>";

    html += "<div class='container'>";  // Start container to center the content and add padding

    // Add existing information about human presence
    html += "<div class='section'><h2>Existing Information:</h2>";
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
    html += "</div>";

    // Add motion information
    html += "<div class='section'><h2>Motion Information:</h2>";
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
    html += "</div>";

    
    html += "<div class='section'><h2>BLE Alert:</h2>";
    html += "<p>Sent</p>";
    html += "</div>";


    // Add fall status
    html += "<div class='section'><h2>Fall Status:</h2>";
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
    html += "</div>";

    // Add PIR sensor status
    html += "<div class='section'><h2>PIR Sensor Status:</h2>";
    int pirValue = digitalRead(PIR_PIN);
    html += "<p>" + String(pirValue == HIGH ? "Motion Detected" : "No Motion") + "</p>";
    html += "</div>";

    html += "</div></body></html>";  // Close container and body

    // Configure the session
    ESP_Mail_Session session;
    session.server.host_name = smtpServer;
    session.server.port = smtpServerPort;
    session.login.email = emailSenderAccount;
    session.login.password = emailSenderPassword;
    session.login.user_domain = "";

    // Configure the message
    SMTP_Message message;
    message.sender.name = "ESP32";
    message.sender.email = emailSenderAccount;
    message.subject = emailSubject;
    message.addRecipient("Recipient", inputMessage);
    message.html.content = html.c_str();  // Use HTML content
    message.html.charSet = "utf-8";
    message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
    message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_high;

    // Set the callback function
    smtp.callback(smtpCallback);

    // Connect to the server and send the email
    if (!smtp.connect(&session)) {
        Serial.println("Failed to connect to SMTP server: " + smtp.errorReason());
        return false;
    }

    if (!MailClient.sendMail(&smtp, &message)) {
        Serial.println("Error sending Email: " + smtp.errorReason());
        smtp.closeSession();
        return false;
    }

    smtp.closeSession();
    return true;
}


// Callback function for email sending status
void smtpCallback(SMTP_Status status)
{
  Serial.println(status.info());
  if (status.success())
  {
    Serial.println("Email sent successfully");
    Serial.println("----------------");
  }
}
