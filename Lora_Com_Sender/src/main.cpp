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

int counter = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  delay(1500);
  Serial.println("LoRa Sender");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  timeClient.begin();

  LoRa.setPins(PIN_LORA_CS, PIN_LORA_RST, PIN_LORA_DIO0);
  LoRa.setSPIFrequency(20000000);
  LoRa.setTxPower(20);

  if (!LoRa.begin(LORA_FREQUENCY)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  } else {
    Serial.print("LoRa initialized with frequency ");
    Serial.println(LORA_FREQUENCY);
  }
}

void loop() {
  timeClient.update();
  String formattedTime = timeClient.getFormattedTime();
  
  Serial.print("Sending packet: ");
  Serial.print(counter);
  Serial.print(" at ");
  Serial.println(formattedTime);

  LoRa.beginPacket();
  LoRa.print("Hello LoRa ");
  LoRa.print(counter);
  LoRa.print(", Timestamp: ");
  LoRa.print(formattedTime);
  LoRa.endPacket();

  counter++;

  delay(1000);
}