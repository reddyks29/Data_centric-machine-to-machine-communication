#include <ArduinoJson.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>  // Include WiFi UDP library

const char* ssid = "ZERO";  // Replace with your WiFi SSID
const char* password = "FCUKOFF!";  // Replace with your WiFi password
IPAddress serverIP(192, 168, 0, 111);  // Raspberry Pi server IP address
unsigned int serverPort = 12347;  // UDP server port on Raspberry Pi

// Defines Trig and Echo pins of the Ultrasonic Sensor
const int trigPin = D7;
const int echoPin = D8;

const int trigPin1 = D0;
const int echoPin1 = D1;

const int trigPin2 = D2;
const int echoPin2 = D3;

// Variables for duration and distance
long duration;
int distance;
Servo myServo;  // Creates a servo object for controlling the servo motor

WiFiUDP udp;  // Create a UDP object

void setup() {
  pinMode(trigPin, OUTPUT);  // Set the trigPin as an Output
  pinMode(echoPin, INPUT);  // Set the echoPin as an Input
  pinMode(trigPin1, OUTPUT);  // Set the trigPin1 as an Output
  pinMode(echoPin1, INPUT);  // Set the echoPin1 as an Input
  pinMode(trigPin2, OUTPUT);  // Set the trigPin2 as an Output
  pinMode(echoPin2, INPUT);  // Set the echoPin2 as an Input

  Serial.begin(9600);
  myServo.attach(D5);  // Define on which pin the servo motor is attached

  WiFi.begin(ssid, password);  // Connect to WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  udp.begin(8888);  // Start UDP on port 8888
}

void loop() {
  // Rotates the servo motor from 15 to 165 degrees
  for (int i = 15; i <= 165; i++) {
    myServo.write(i);
    delay(10); // Decreased delay for faster movement
    distance = calculateDistance(trigPin, echoPin);  // Calculate distance measured by the Ultrasonic sensor for each degree
    sendUDPData(i, distance);  // Send data via UDP
  }

  // Repeats the previous lines from 165 to 15 degrees
  for (int i = 165; i > 15; i--) {
    myServo.write(i);
    delay(10); // Decreased delay for faster movement
    distance = calculateDistance(trigPin, echoPin);
    sendUDPData(i, distance);  // Send data via UDP
  }
}

// Function for calculating the distance measured by the Ultrasonic sensor
int calculateDistance(int pin1, int pin2) {
  digitalWrite(pin1, LOW);
  delayMicroseconds(2);
  digitalWrite(pin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin1, LOW);
  duration = pulseIn(pin2, HIGH);  // Read the echoPin, returns the sound wave travel time in microseconds
  distance = duration * 0.034 / 2;
  return distance;
}

// Function to send data via UDP
void sendUDPData(int angle, int distance) {
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["servo_angle"] = angle;
  jsonDoc["servo_distance"] = distance;
  jsonDoc["water_lvl"] = calculateDistance(trigPin1, echoPin1);
  jsonDoc["parking"] = calculateDistance(trigPin2, echoPin2);

  String jsonString;
  serializeJson(jsonDoc, jsonString);
  Serial.println(jsonString);

  udp.beginPacket(serverIP, serverPort);  // Begin packet to server
  udp.print(jsonString);  // Write data to packet
  udp.endPacket();  // Send packet
}