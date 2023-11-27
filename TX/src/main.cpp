#include <SPI.h>
#include <LoRa.h>
#include <lmic.h>
#include <hal/hal.h>

#define SS_PIN 7
#define RST_PIN 1
#define DI0_PIN 2
#define frequency 915E6

const lmic_pinmap lmic_pins = {
    .nss = 7,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 1,
    .dio = {2, 6, LMIC_UNUSED_PIN},
};


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
  LoRa.write((byte*)&value, sizeof(value));  // Send the binary representation of the integer
  LoRa.endPacket();
}