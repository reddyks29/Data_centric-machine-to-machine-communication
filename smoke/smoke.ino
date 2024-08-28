#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
int mq2 = A0;
int data = 0;

const char* ssid = "ZERO";
const char* password = "FCUKOFF!";
const char* udpAddress = "192.168.0.111";
const int udpPort = 12345;

WiFiUDP udp;

void setup()
{
  Serial.begin(9600);
  WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
}

void loop()
{
  data = analogRead(mq2);
  Serial.print("Smoke level: ");
  Serial.println(data);
  
  if (data > 700) {
    Serial.print("High smoke level detected: ");
    Serial.println(data);  
  }
  udp.beginPacket(udpAddress, udpPort);
  udp.printf("Smoke level: %d", (int)data);
  udp.endPacket();
  delay(1000);
}
