#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <DHT.h>
#include <Servo.h>

int pressure;

bool oned = false;
#define PIN_PIZO D0
#define PIN_ENA  D5     // The ESP8266 pin connected to the EN1 pin L298N
#define PIN_IN1  D6    // The ESP8266 pin connected to the IN1 pin L298N
#define PIN_IN2  D7   // The ESP8266 pin connected to the IN2 pin L298N
#define PIN_SERVO D1 // The ESP8266 pin connected to the servo motor
#define PIN_IR D4
#define PIN_FSR A0

#define DHTTYPE DHT11
#define dht_dpin D8
DHT dht(dht_dpin, DHTTYPE);

const char* ssid = "This";
const char* password = "12345668";
const char* udpAddress = "192.168.72.218";
const int udpPort = 12346;

WiFiUDP udp;
Servo servoMotor;

unsigned long motorStartTime = 0;
unsigned long motorDuration = 5000; 
bool motorRunning = false;

void call_fun(){
  while (digitalRead(PIN_IR) == LOW){
    Serial.println("Hand on sensor");
    delay(500);
  }
  if (oned == false){
    oned = true;
    servoMotor.write(90);
  }
  else if (oned == true){
    oned = false;
    servoMotor.write(0);
  }
  return;
}

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
  pinMode(PIN_SERVO, OUTPUT);
  pinMode(PIN_IR,INPUT);

  dht.begin();
  servoMotor.attach(PIN_SERVO);
}

void loop() {
  unsigned long currentTime = millis();
  if (dht.readTemperature()>35.00){
    startMotor();
  }
  else{
    stopMotor();
  } 
  
  if (digitalRead(PIN_IR) == LOW ) {
    call_fun(); 
  }
  //cooperative multitasking
  fsrTask();
  dhtTask();
  delay(1000);
}

void startMotor() {
  if (!motorRunning){
    digitalWrite(PIN_IN1, HIGH);
    digitalWrite(PIN_IN2, LOW);
    int motorSpeed = 200;
    analogWrite(PIN_ENA, motorSpeed);
    motorStartTime = millis();
    motorRunning = true;
  }
}

void stopMotor() {
    if (motorRunning){  
      analogWrite(PIN_ENA, 0);
      motorRunning = false;
    }
}

void fsrTask() {
  pressure = analogRead(PIN_FSR); 
  Serial.println(pressure);
  if (pressure < 200){
      digitalWrite(PIN_PIZO,HIGH);
  }
  else{
      digitalWrite(PIN_PIZO,LOW);
  }
}

void dhtTask() {
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["humidity"] = dht.readHumidity();
  jsonDoc["temperature"] = dht.readTemperature();
  jsonDoc["pressure"] = analogRead(PIN_FSR);
  if( oned == true ){
    jsonDoc["window"] = 1;
  }else{
    jsonDoc["window"] = 0;
  }
  if( motorRunning == true ){
    jsonDoc["dc_motor"] = 1;
  }else{
    jsonDoc["dc_motor"] = 0;
  }
  
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  Serial.println(jsonString);
  udp.beginPacket(udpAddress, udpPort);
  udp.print(jsonString);

  udp.endPacket();
}