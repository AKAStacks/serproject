// IR Project
// This code is without warranty and public domain

#define _IR_ENABLE_DEFAULT_ false
#define SEND_SAMSUNG true

#ifndef STASSID
#define STASSID "MonkeyMan"
#define STAPSK "3012773249"

#include "ircodes.h"
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Samsung.h>

#include <ESP8266WebServer.h>
#include <Uri.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 13 // ESP32 DOES NOT DEFINE LED_BUILTIN
#endif

int LED = LED_BUILTIN;
ESP8266WebServer server(80);

const char* ssid = STASSID;
const char* password = STAPSK;

#define IRLED D5  // GPIO 14 (D5)

IRsend irsend(IRLED);
// Delay between ticks in ms
const int thoughtDelay = 1000;

// Length of command chains (static for now)
const int arrayLength = 37;
enum BrightAdjust { up, down, none };

String inputString = "";
bool stringComplete = false;

bool inCommandChain = false;
BrightAdjust brightAdjust = none;
int currentCommandStep = 0;

unsigned long nextThought = 0;

void announce(String message) {
 Serial.println("> " + message);
 Serial.flush();
}

void handleSerial() {
  if (Serial.available()) {
    if (Serial.peek() == '>') {
      return;
    }
    digitalWrite(LED_BUILTIN, HIGH);
    inputString = Serial.readString();
    stringComplete = true;
  }
  if (stringComplete) {
    inputString.trim();
    if (inputString == "toggle" || inputString == "ߗ") {
      irsend.sendSAMSUNG(SamsungPowerToggle);
      /* announce("OK, turning the TV off..."); */
    } else if (inputString == "exit") {
      inCommandChain = false;
      brightAdjust = none;
      currentCommandStep = 0;
      irsend.sendSAMSUNG(sExit);
      irsend.sendSAMSUNG(sExit);
      irsend.sendSAMSUNG(sExit);
    } else if (inputString == "daylight") {
      inCommandChain = true;
      brightAdjust = up;
      /* announce("OK, setting TV to day mode."); */
    } else if (inputString == "nightlight") {
      inCommandChain = true;
      brightAdjust = down;
      /* announce("OK, setting TV to night mode."); */
    } else {
      while (Serial.available() > 0) {
        Serial.read();
      }
   }
    digitalWrite(LED_BUILTIN, LOW);
    stringComplete = false;
    inputString = "";
  }
}

void handleCommandChain() {
  if (inCommandChain) {
    // If we've taken all the steps, reset
    if (currentCommandStep >= arrayLength) {
      inCommandChain = false;
      currentCommandStep = 0;
      brightAdjust = none;
    } else {
      if (brightAdjust == up) {
        irsend.sendSAMSUNG(dayLight[currentCommandStep]);
      } else if (brightAdjust == down) {
        irsend.sendSAMSUNG(nightLight[currentCommandStep]);
      }
      // announce(String(currentCommandStep));
      currentCommandStep++;
    }
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(IRLED, OUTPUT);
  pinMode(MOTOR, OUTPUT);

  inputString.reserve(200);

  # We need to connect to WiFi and set up mDNS here
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    announce("Awaiting WiFi connection... ")
  }

  announce("Connected to: ");
  announce(ssid);
  announce("IP address: ");
  announce(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
	announce("mDNS responder started");
  }
  
  irsend.begin();
  /* announce("Waking up... ahh..."); */
}

void loop() {
  if (nextThought == 0) {
    /* announce("Having my first thought--this is awful."); */
  }
  if (millis() >= nextThought) {
    nextThought += thoughtDelay;
	# We need to add a function to handle HTTP requests
    handleCommandChain();
    handleSerial();
  }
  MDNS.update();
}
