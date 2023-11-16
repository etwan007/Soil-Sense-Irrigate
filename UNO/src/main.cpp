//Libraries
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <LoRa.h>
#include <SPI.h>
#include <SD.h>

//SD Card
char sdPin = 10;
File myFile;

//LED Variables
int ledG = 2;
int ledY = 3;
int ledR = 4;

//LoRa Communication
#define SS_PIN 10
#define RST_PIN 9
#define DI0_PIN 2
#define frequency 433E6

//Moisture
int moistureValue = 0;

//Function Prototypes
void dataLog();


void setup() {
  Serial.begin(9600);  // Initialize the Serial communication
  delay(10);

  Serial.println("LoRa Receiver");

  // Setup LoRa module
  LoRa.setPins(SS_PIN, RST_PIN, DI0_PIN);
  while (!LoRa.begin(frequency)) {
    Serial.println("LoRa initialization failed. Check your wiring.");
    delay(5000);
  }
  Serial.println("LoRa initialization successful!");

  //SD card
  while (!SD.begin(sdPin)) {
    Serial.println("Not Initialized");    //Debugging
    delay(1000);
  }
  Serial.println("SD initialized");   //Debugging

  pinMode(ledG, OUTPUT);  // Set the LED pin as an output
  pinMode(ledY, OUTPUT);  // Set the LED pin as an output
  pinMode(ledR, OUTPUT);  // Set the LED pin as an output
}

void loop() {
 int packetSize = LoRa.parsePacket();

  if (packetSize) {
    Serial.print("Received packet: ");

    // Read the packet data
    while (LoRa.available()) {
      moistureValue = LoRa.read();
      moistureValue = moistureValue << 8 | LoRa.read();  // Combine bytes
      Serial.print(moistureValue);
    }

    Serial.println();
  }

  dataLog(moistureValue);

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

    delay(1000);
}

void dataLog(int moistureValue) {
  // Open the file in write mode
  myFile = SD.open("data/moisture.txt", FILE_WRITE);

  // Check if the file opened successfully
  if (myFile) {
    // Write the moisture value to the file
    myFile.println(String(moistureValue) + " ");
    
    // Close the file
    myFile.close();
  } else {
    // If the file couldn't be opened, print an error message
    Serial.println("Error opening file for writing");   //DeBugging
  }
}