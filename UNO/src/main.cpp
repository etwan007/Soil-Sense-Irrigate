//Libraries
#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h>
#include <SD.h>
#include <lmic.h>
#include <hal/hal.h>

//SD Card
char sdPin = 10;
File myFile;

//LED Variables
// int ledG = 2;
// int ledY = 3;
// int ledR = 4;

//LoRa Communication
#define SS_PIN 10
#define RST_PIN 9
#define DI0_PIN 2
const lmic_pinmap lmic_pins = {
    .nss = 10,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 9,
    .dio = {2, 6, LMIC_UNUSED_PIN},
};
#define frequency 433E6
static const u1_t DEVEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const u1_t APPEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const u1_t APPKEY[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8); }
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8); }
void os_getDevKey (u1_t* buf) { memcpy_P(buf, APPKEY, 16); }

static osjob_t sendjob;
// User-defined data to send
static const char* payload = "Hello, TTN!";

//Moisture
int moistureValue = 0;

//Function Prototypes
void dataLog(int);
void do_send(osjob_t* j);
void onEvent (ev_t ev);


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

  // LMIC init
  os_init();

  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();

  // Start job (sending automatically starts OTAA too)
  do_send(&sendjob);

  //SD card
  while (!SD.begin(sdPin)) {
    Serial.println("Not Initialized");    //Debugging
    delay(1000);
  }
  //Serial.println("SD initialized");   //Debugging

  // pinMode(ledG, OUTPUT);  // Set the LED pin as an output
  // pinMode(ledY, OUTPUT);  // Set the LED pin as an output
  // pinMode(ledR, OUTPUT);  // Set the LED pin as an output
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

  //Send the Data
  os_runloop_once();

    // if (moistureValue > 60) //Moisture is ok
    // {
    //   digitalWrite(ledG, HIGH);
    //   digitalWrite(ledY, LOW);
    //   digitalWrite(ledR, LOW);
    // }
    // else if (moistureValue > 30) //MOisture is close
    //  {
    //   digitalWrite(ledG, LOW);
    //    digitalWrite(ledY, HIGH);
    //   digitalWrite(ledR, LOW);
    //  }
    // else //Needs sprinklers
    //  {
    //   digitalWrite(ledG, LOW);
    //   digitalWrite(ledY, LOW);
    //   digitalWrite(ledR, HIGH);
    // }

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


void do_send(osjob_t* j) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println("OP_TXRXPEND, not sending");
  } else {
    // Prepare upstream data transmission at the next possible time.
    LMIC_setTxData2(1, (uint8_t*)payload, strlen(payload), 0);
    Serial.println("Packet queued");
  }
}

void onEvent (ev_t ev) {
  switch(ev) {
    case EV_TXCOMPLETE:
      Serial.println("EV_TXCOMPLETE (includes waiting for RX windows)");
      if (LMIC.txrxFlags & TXRX_ACK)
        Serial.println("Received ack");
      if (LMIC.dataLen) {
        Serial.println("Received ");
        Serial.println(LMIC.dataLen);
        Serial.println(" bytes of payload");
      }
      // Schedule next transmission
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(5), do_send);
      break;
    default:
      break;
  }
}