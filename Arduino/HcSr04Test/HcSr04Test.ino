#include <WiFi.h>
#include <WebServer.h>
#include "dashboard.h"

// Define network credentials
const char* ssid = "amongussussus800858===D";
const char* password = "milanstfu";

// HC-SR04 Pins
const int trigPin = 5;
const int echoPin = 18;

// LED Pin Control
const int ledPin = 2; // Onboard LED

// Sound velocity in cm/uS
#define SOUND_SPEED 0.034

long duration;
float distanceCm;
bool ledState = false;

WebServer server(80);

void setup() {
  Serial.begin(115200);

  // Setup pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Setup Web Server Routes
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", dashboard_html);
  });

  server.on("/distance", HTTP_GET, []() {
    String json = "{\"distance\": " + String(distanceCm) + "}";
    server.send(200, "application/json", json);
  });

  server.on("/toggle", HTTP_POST, []() {
    ledState = !ledState;
    digitalWrite(ledPin, ledState ? HIGH : LOW);
    String json = "{\"state\": " + String(ledState ? "true" : "false") + "}";
    server.send(200, "application/json", json);
  });

  server.on("/state", HTTP_GET, []() {
    String json = "{\"state\": " + String(ledState ? "true" : "false") + "}";
    server.send(200, "application/json", json);
  });

  server.begin();
  Serial.println("Web server started");
}

void loop() {
  // Handle web client requests
  server.handleClient();

  // Read HC-SR04
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_SPEED / 2;
  
  delay(100); // 100ms reading interval
}
