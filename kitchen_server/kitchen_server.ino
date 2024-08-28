#include <ArduinoJson.h>  // Include ArduinoJSON library
#include <WiFi.h>
#include <WiFiUdp.h>
#include <DHT.h>

#define DHTTYPE DHT11
#define dht_dpin 15
DHT dht(dht_dpin, DHTTYPE);

#define IR_Sensor 14
#define Gas_Sensor 34
#define LED_BUILTIN 2

int IR;
bool oned = false;

const char* ssid = "This";
const char* password = "12345668";
const char* udpAddress = "192.168.72.218";
const int udpPort = 12345;

WiFiUDP udp;

TaskHandle_t TaskIR, TaskGas;

void IR_Task(void* pvParameters) {
  pinMode(IR_Sensor, INPUT);
  while (true) {
    IR = digitalRead(IR_Sensor);
    if (IR == LOW) {
      call_fun();
    }
    vTaskDelay(100); // Delay to avoid continuous checking
  }
}

void Gas_Task(void* pvParameters) {
  pinMode(Gas_Sensor, INPUT);
  int gasData;
  while (true) {
    gasData = analogRead(Gas_Sensor);
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["humidity"] = dht.readHumidity();
    jsonDoc["temperature"] = dht.readTemperature();
    jsonDoc["smoke_level"] = gasData;

    // Serialize the JSON object into a string
    String jsonString;
    serializeJson(jsonDoc, jsonString);
    Serial.print(jsonString);
    udp.beginPacket(udpAddress, udpPort);
    udp.print(jsonString); // Send JSON string over UDP
    udp.endPacket();

    vTaskDelay(1000); // Delay to read data every second
  }
}
void initiate_wifi(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  
  initiate_wifi();
  dht.begin();

  xTaskCreate(IR_Task, "IR Task", 2048, NULL, 1, &TaskIR);
  xTaskCreate(Gas_Task, "Gas Task", 2048, NULL, 1, &TaskGas);
}

void loop() {
  // No need to send data in loop() as it's handled in Gas_Task
  delay(1000);
}
void call_fun() {
  while (digitalRead(IR_Sensor) == LOW) {
    Serial.println("Hand on sensor");
  }
  if (oned == false) {
    oned = true;
    digitalWrite(LED_BUILTIN, HIGH);
  } else if (oned == true) {
    oned = false;
    digitalWrite(LED_BUILTIN, LOW);
  }
}
