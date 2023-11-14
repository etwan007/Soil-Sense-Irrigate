#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

//Wifi Variables
const char* ssid = "ReceiverAP"; 
//const char* password = "Password";
const char* receiverIP = "192.168.4.1";

//Moisture Variables
const int _dry = 1024; // value for dry sensor
const int _wet = 560; // value for wet sensor
int sensorPin = A0;
int sensorValue;
int percentageMoisture;

//Sender ID
const String senderID = "Transmitter1";

//Function Prototypes
void sendDataToReceiver();

void setup() {
  Serial.begin(9600);
  delay(10);

  // Connect to the Receiver's AP
  WiFi.begin(ssid);       //add password here if want
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to the Receiver AP");

  sendDataToReceiver();
}

void loop() {
  sendDataToReceiver();
  delay(500);
}

void sendDataToReceiver() {
  HTTPClient http;
  WiFiClient client;
  
  sensorValue = analogRead(sensorPin);
  Serial.println(sensorValue);
  percentageMoisture = map(sensorValue, _dry, _wet, 0, 100); // Reverse mapping for percentage
  Serial.println(percentageMoisture);

  // Create the URL as a single 
  String url = "http://";
  url += receiverIP;
  url += "/data";
  
  http.begin(client, url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  // Prepare the data to send
  String dataToSend = "senderID=" + senderID + "&moisture=" + String(percentageMoisture);

  // Send the data to the receiver
  int httpResponseCode = http.POST(dataToSend);
  if (httpResponseCode > 0) {
    String response = http.getString();
    //Serial.println("Data sent to Receiver, Response: " + response);
  } else {
    Serial.println("Error sending data to Receiver");
  }

  http.end();
}