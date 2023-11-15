#include <SPI.h>
#include <LoRa.h>

#define SS_PIN 10
#define RST_PIN 9
#define DI0_PIN 2
#define frequency 433E6

void sendMessage(int value);

void setup() {
  Serial.begin(9600);
  
  // Setup LoRa module
  LoRa.setPins(SS_PIN, RST_PIN, DI0_PIN);
  while (!LoRa.begin(frequency)) {
    Serial.println("LoRa initialization failed. Check your wiring.");
    delay(1000);
  }
  Serial.println("LoRa initialization successful!");
}

void loop() {
  // Send a value
  int sensorValue = analogRead(A0);
  sendMessage(sensorValue);

  delay(1000);
}

void sendMessage(int value) {
  Serial.print("Sending value: ");
  Serial.println(value);

  // Send packet
  LoRa.beginPacket();
  LoRa.print(value);
  LoRa.endPacket();
}