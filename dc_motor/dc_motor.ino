#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <DHT.h>
#include <Servo.h>


const int sensorOut = A0;
int pressure;

#define PIN_PIZO D0
#define PIN_ENA  D5 // The ESP8266 pin connected to the EN1 pin L298N
#define PIN_IN1  D6 // The ESP8266 pin connected to the IN1 pin L298N
#define PIN_IN2  D7 // The ESP8266 pin connected to the IN2 pin L298N

#define DHTTYPE DHT11
#define dht_dpin D8
DHT dht(dht_dpin, DHTTYPE);

const char* ssid = "ZERO";
const char* password = "FCUKOFF!";
const char* udpAddress = "192.168.0.111";
const int udpPort = 12345;

WiFiUDP udp;

unsigned long motorStartTime = 0;
unsigned long motorDuration = 5000; // 5 seconds
bool motorRunning = false;

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_ENA, OUTPUT);
  pinMode(PIN_PIZO, OUTPUT);
  dht.begin();
}

void loop() {
  unsigned long currentTime = millis();

  // Check if it's time to start or stop the motor
  if (!motorRunning && currentTime - motorStartTime >= motorDuration) {
    startMotor();
  } else if (motorRunning && currentTime - motorStartTime >= motorDuration + 1000) {
    stopMotor();
  }
  if (analogRead(sensorOut)<150){
    digitalWrite(PIN_PIZO,HIGH);
  }else{
    digitalWrite(PIN_PIZO,LOW);
  }
  // Run other tasks concurrently using cooperative multitasking
  fsrTask();
  dhtTask();
  delay(1000);
}

void startMotor() {
  digitalWrite(PIN_IN1, HIGH); // control the motor's direction in clockwise
  digitalWrite(PIN_IN2, LOW);
  int motorSpeed = 200;
  analogWrite(PIN_ENA, motorSpeed);
  motorStartTime = millis();
  motorRunning = true;
}

void stopMotor() {
  analogWrite(PIN_ENA, 0);
  motorRunning = false;
}

void fsrTask() {
  pressure = analogRead(sensorOut); // Read and store analog value from Force Sensitive Resistance
  Serial.println(pressure);
}

void dhtTask() {
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["humidity"] = dht.readHumidity();
  jsonDoc["temperature"] = dht.readTemperature();
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  Serial.println(jsonString);
}
