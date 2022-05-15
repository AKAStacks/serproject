// IR Project
// This code is without warranty and public domain

#define _IR_ENABLE_DEFAULT_ false
#define SEND_SAMSUNG true

#ifndef STASSID
#define STASSID "MonkeyMan"
#define STAPSK "3012773249"
#endif

#include "ircodes.h"
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Samsung.h>

#include <ESP8266WebServer.h>
#include <Uri.h>
//#include <ESP8266mDNS.h>

#include <WiFiClient.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 13 // ESP32 DOES NOT DEFINE LED_BUILTIN
#endif

#define DEBUG 1

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
  if (DEBUG == 1) {
    Serial.println("> " + message);
    flashLED();
    Serial.flush();
  }
}

void flashLED() {
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
}

void handleSerial() {
  if (Serial.available()) {
    if (Serial.peek() == '>') {
      return;
    }
    inputString = Serial.readString();
    stringComplete = true;
  }
  if (stringComplete) {
    flashLED();
    inputString.trim();
    if (inputString == "toggle" || inputString == "ߗ") {
      tvToggle();
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
      announce("OK, setting TV to day mode.");
    } else if (inputString == "nightlight") {
      inCommandChain = true;
      brightAdjust = down;
      announce("OK, setting TV to night mode.");
    } else {
      while (Serial.available() > 0) {
        Serial.read();
      }
   }
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
      announce(String(currentCommandStep));
      currentCommandStep++;
    }
  }
}

void handleRoot() {
  server.send(200, "text/html",
  "<html>\
    <body style='background: black; overflow: hidden;'>\
        <div style='width: 100vw; height: 100vh; display: grid; place-items: center; color: white;'>\
        Ayyo was good\
        </br>this post brought to you by @emacs arduino-mode\
        </br>\
        <button type='button' onclick='toggleLED();'>Toggle LED</button>\
        <script>\
            var ledState = 0;\
            function toggleLED() {\
              let xhttp = new XMLHttpRequest();\
              xhttp.onreadystatechange = function() {\
                if (this.readyState == 4 && this.status == 200) {\
                    if (ledState === 0) {\
                        ledState = 1;\
                    } else {\
                        ledState = 0;\
                    };\
                }\
              };\
              xhttp.open('POST', 'ajaxinfo.txt', true);\
              xhttp.send();\
            };\
        </script>\
      </div>\
     </body>\
   </html>");
}

void setup() {
  Serial.begin(9600);

  pinMode(IRLED, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  inputString.reserve(200);

  // We need to connect to WiFi and set up mDNS here
  WiFi.mode(WIFI_STA);
  WiFi.hostname("tvctl");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    announce("Awaiting WiFi connection... ");
  }

  announce("Connected to: ");
  announce(ssid);
  announce("IP address: ");
  announce(WiFi.localIP().toString().c_str());

/*  if (MDNS.begin("tvctl")) {
	  announce("mDNS responder started");
  } */

  server.on("/", handleRoot);
  server.on("/ajaxinfo.txt", tvToggle);
  server.begin();
  
  irsend.begin();
  announce("Waking up... ahh...");
}

void tvToggle() {
  irsend.sendSAMSUNG(SamsungPowerToggle);
  announce("OK, turning the TV off...");
}

void loop() {
  if (nextThought == 0) {
    announce("Having my first thought--this is awful.");
  }
  if (millis() >= nextThought) {
	// announce("Having a thought");
    nextThought += thoughtDelay;
	// We need to add a function to handle HTTP requests
    handleCommandChain();
    handleSerial();
  }
  server.handleClient();
  //MDNS.update();
}
