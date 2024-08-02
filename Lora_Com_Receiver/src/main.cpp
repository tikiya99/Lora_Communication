#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <SPI.h>
#include <LoRa.h>

#define PIN_LORA_COPI   23
#define PIN_LORA_CIPO   19
#define PIN_LORA_SCK    18
#define PIN_LORA_CS     5
#define PIN_LORA_RST    2
#define PIN_LORA_DIO0   4

#define LORA_FREQUENCY  433E6

const char* ssid     = "your_SSID";
const char* password = "your_PASSWORD";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

void setup() {
  Serial.begin(115200);
  while (!Serial);
  delay(1500);
  Serial.println("LoRa Receiver");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  timeClient.begin();

  LoRa.setPins(PIN_LORA_CS, PIN_LORA_RST, PIN_LORA_DIO0);
  LoRa.setSPIFrequency(20000000);

  if (!LoRa.begin(LORA_FREQUENCY)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  } else {
    Serial.print("LoRa initialized with frequency ");
    Serial.println(LORA_FREQUENCY);
  }
}

void loop() {
  int packetSize = LoRa.parsePacket();
  
  if (packetSize) {
    Serial.print("Received packet '");

    String receivedData = "";
    while (LoRa.available()) {
      receivedData += (char) LoRa.read();
    }

    Serial.print(receivedData);
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());

    sendAcknowledgment(receivedData);
  }
}

void sendAcknowledgment(String receivedData) {
  timeClient.update();
  String formattedTime = timeClient.getFormattedTime();
  String ackMessage = "ACK: " + receivedData + ", Timestamp: " + formattedTime;
  
  LoRa.beginPacket();
  LoRa.print(ackMessage);
  LoRa.endPacket();

  Serial.print("Sent acknowledgment: ");
  Serial.println(ackMessage);
}
