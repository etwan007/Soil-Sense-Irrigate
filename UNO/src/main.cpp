#include <Arduino.h>
#include <SoftwareSerial.h>
int ledG = 2;
int ledY = 3;
int ledR = 4;


void setup() {
  Serial.begin(9600);  // Initialize the Serial communication
  delay(10);

  pinMode(ledG, OUTPUT);  // Set the LED pin as an output
  pinMode(ledY, OUTPUT);  // Set the LED pin as an output
  pinMode(ledR, OUTPUT);  // Set the LED pin as an output
}

void loop() {
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');  // Read data until newline character

    // Assuming the data received is in the format "Moisture: XX"
    if (data.startsWith("Moisture: ")) {
      int moistureValue = data.substring(10).toInt();  // Extract the moisture value
        if (moistureValue > 60) //Moisture is ok
        {
          digitalWrite(ledG, HIGH);
          digitalWrite(ledY, LOW);
          digitalWrite(ledR, LOW);
        }
        else if (moistureValue > 30) //MOisture is close
        {
          digitalWrite(ledG, LOW);
          digitalWrite(ledY, HIGH);
          digitalWrite(ledR, LOW);
        }
        else //Needs sprinklers
        {
          digitalWrite(ledG, LOW);
          digitalWrite(ledY, LOW);
          digitalWrite(ledR, HIGH);
        }
    }
  }
}

