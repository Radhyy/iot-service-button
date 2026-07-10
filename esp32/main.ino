#include <WiFi.h>
#include <HTTPClient.h>

// ==========================================
// CONFIGURATION
// ==========================================
// CHANGE THIS NUMBER FOR EACH DEVICE (1 device per student)
const int STUDENT_ID = 1;

// WiFi Credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Your Netlify URL (Don't forget the https:// and /api/record-button)
// Example: https://my-iot-backend.netlify.app/api/record-button
const char* serverName = "https://iotservicebutton.netlify.app/api/record-button";

// Pin Definitions
const int PIN_SUCCESS = 14;  // Green Button
const int PIN_STRUGGLE = 27; // Yellow Button
const int PIN_HELP = 26;     // Red Button

// Debounce timing
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 500; // 500ms between presses
// ==========================================

void setup() {
  Serial.begin(115200);

  // Initialize pins as inputs with internal pull-up resistors
  pinMode(PIN_SUCCESS, INPUT_PULLUP);
  pinMode(PIN_STRUGGLE, INPUT_PULLUP);
  pinMode(PIN_HELP, INPUT_PULLUP);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Read button states (LOW means pressed due to INPUT_PULLUP)
  bool successPressed = (digitalRead(PIN_SUCCESS) == LOW);
  bool strugglePressed = (digitalRead(PIN_STRUGGLE) == LOW);
  bool helpPressed = (digitalRead(PIN_HELP) == LOW);

  unsigned long currentMillis = millis();

  if ((currentMillis - lastDebounceTime) > debounceDelay) {
    if (successPressed) {
      sendInteraction("SUCCESS");
      lastDebounceTime = currentMillis;
    } 
    else if (strugglePressed) {
      sendInteraction("STRUGGLE");
      lastDebounceTime = currentMillis;
    } 
    else if (helpPressed) {
      sendInteraction("HELP");
      lastDebounceTime = currentMillis;
    }
  }
}

void sendInteraction(String buttonType) {
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    http.begin(serverName);

    // Specify content-type header
    http.addHeader("Content-Type", "application/json");

    // Prepare JSON payload
    String payload = "{\"student_id\":" + String(STUDENT_ID) + ", \"button_type\":\"" + buttonType + "\"}";
    Serial.println("Sending payload: " + payload);

    // Send HTTP POST request
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String response = http.getString();
      Serial.println(response);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}
