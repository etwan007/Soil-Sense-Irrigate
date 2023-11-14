#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "LittleFS.h"
#include <SoftwareSerial.h>

//WiFi Variables
const char* ssid = "ReceiverAP";

//Moisture Variables
String senderID = "No Id";
String moisture = "No Data";
String responseMessage = "None";

//Function Prototypes
void handleData();
void handleRoot();
void handleDataRequest();
void handleJS();

//Server
ESP8266WebServer server(80);

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  delay(10);

  //Initiate WiFi
  WiFi.softAP(ssid);
  delay(500);

  //Debugging
  Serial.print("Receiver AP IP address: ");
  Serial.println(WiFi.softAPIP());

  //Mount File
  if (!LittleFS.begin()) {
    //Serial.println("Failed to mount file system");
    return;
  }

  server.on("/data", HTTP_POST, handleData);
  server.on("/data", HTTP_GET, handleDataRequest);
  server.on("/", HTTP_GET, handleRoot);
  server.on("/script.js", HTTP_GET, handleJS);
  server.begin();
}

void loop() {
  server.handleClient();
  handleData();
  delay(500);
}

void handleData() {
  if (server.hasArg("senderID") && server.hasArg("moisture")) {
    senderID = server.arg("senderID");
    moisture = server.arg("moisture");

    //Serial.println("Received data from Sender ID: " + senderID);
    //Serial.println("Data: " + moisture);
    Serial1.println("Moisture: " + moisture);
    Serial.println("Moisture: " + moisture);

    responseMessage = F("Data received by Receiver\n");
    responseMessage += "Sender ID: " + senderID + "\n";
    responseMessage += "Moisture: " + moisture + "\n";

    server.send(200, "text/plain", "Data received by Receiver");
  }
}

void handleRoot() {
  File file = LittleFS.open("/index.html", "r");
  if (file) {
    server.streamFile(file, "text/html");
    file.close();
  } else {
    server.send(404, "text/plain", "File Not Found");
  }
}

void handleDataRequest() {
  server.send(200, "text/plain", moisture + server.arg("moisture"));
}

void handleJS() {
  File jsFile = LittleFS.open("/script.js", "r");
  if (jsFile) {
    server.streamFile(jsFile, "application/javascript");
    jsFile.close();
  } else {
    server.send(404, "text/plain", "File Not Found");
  }
}