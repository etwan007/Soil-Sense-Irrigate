#include <Arduino.h>
#include <lmic.h>
#include <hal/hal.h>
#include <LoRa.h>
#include <SD.h>
#include <SPI.h>

//SD Card
char sdPin = 8;
File myFile;

// // TTN Device and Application EUI, Application Key
// static const u1_t DEVEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
// static const u1_t APPEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
// static const u1_t APPKEY[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

const lmic_pinmap lmic_pins = {
    .nss = 7,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 5,
    .dio = {3, 6, LMIC_UNUSED_PIN},
};

#define SS_PIN 7
#define RST_PIN 5
#define DI0_PIN 3
#define frequency 915E6
int moistureValue = 0;
int percentageValue = 0;

int minValue = 350;              // The minimum raw sensor value
int maxValue = 800;           // The maximum raw sensor value
int minPercentage = 0;         // The minimum percentage you want
int maxPercentage = 100;       // The maximum percentage you want



// void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8); }
// void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8); }
// void os_getDevKey (u1_t* buf) { memcpy_P(buf, APPKEY, 16); }

// static osjob_t sendjob;
// void do_send(osjob_t* j);
// void onEvent (ev_t ev);
void dataLog();

void setup() {
  Serial.begin(9600);

  // Setup LoRa module
  LoRa.setPins(SS_PIN, RST_PIN, DI0_PIN);

  pinMode(10, OUTPUT);

  //SD card
  while (!SD.begin(sdPin)) {
    Serial.println("Not Initialized");    //Debugging
    delay(1000);
  }
  Serial.println("SD initialized");   

  while (!LoRa.begin(frequency)) {
    Serial.println("LoRa initialization failed. Check your wiring.");
    delay(1000);
  }
  Serial.println("LoRa initialization successful!");

  // // LMIC init
  // os_init();

  // // Reset the MAC state. Session and pending data transfers will be discarded.
  // LMIC_reset();

  // // Start job (sending automatically starts OTAA too)
  // do_send(&sendjob);
}

void loop() {
digitalWrite(10, HIGH);
   int packetSize = LoRa.parsePacket();

  if (packetSize >= sizeof(int)) {
    Serial.print("Received packet: ");

    // Read the integer value from the packet
    LoRa.readBytes((byte*)&moistureValue, sizeof(moistureValue));

    percentageValue = map(moistureValue, minValue, maxValue, minPercentage, maxPercentage);

    Serial.println(percentageValue); 
    dataLog();
    delay(120000);
  }
  // Send the Data
  //os_runloop_once();
}

// void do_send(osjob_t* j) {
//   // Check if there is not a current TX/RX job running
//   if (LMIC.opmode & OP_TXRXPEND) {
//     Serial.println("OP_TXRXPEND, not sending");
//   } else {
//     // Prepare upstream data transmission at the next possible time.
//     char payloadStr[2];  // Adjust the size based on your integer range
//     sprintf(payloadStr, "%d", moistureValue);
//     LMIC_setTxData2(1, (uint8_t*)payloadStr, strlen(payloadStr), 0);
//     Serial.println("Packet queued");
//   }
// }

// void onEvent (ev_t ev) {
//   switch(ev) {
//     case EV_TXCOMPLETE:
//       Serial.println("EV_TXCOMPLETE (includes waiting for RX windows)");
//       if (LMIC.txrxFlags & TXRX_ACK)
//         Serial.println("Received ack");
//       if (LMIC.dataLen) {
//         Serial.println("Received ");
//         Serial.println(LMIC.dataLen);
//         Serial.println(" bytes of payload");
//       }
//       // Schedule next transmission
//       os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(5), do_send);
//       break;
//     default:
//       break;
//   }
// }

void dataLog() {
  // Open the file in write mode
  myFile = SD.open("moisture.txt", FILE_WRITE);

  // Check if the file opened successfully
  if (myFile) {
    // Write the moisture value to the file
    myFile.println(String(percentageValue) + " ");

    // Close the file
    myFile.close();
  } else {
    // If the file couldn't be opened, print an error message
    Serial.println("Error opening file for writing");   //DeBugging
  }
}