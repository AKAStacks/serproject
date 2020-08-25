// IR Project
// This code is without warranty and public domain

#define _IR_ENABLE_DEFAULT_ false
#define SEND_SAMSUNG true

#include "ircodes.h"
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Samsung.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 13 // ESP32 DOES NOT DEFINE LED_BUILTIN
#endif

int LED = LED_BUILTIN;

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

/* void announce(String message) { */
 // Serial.println("> " + message);
 // Serial.flush();
// }

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

  inputString.reserve(200);

  irsend.begin();
  /* announce("Waking up... ahh..."); */
}

void loop() {
  if (nextThought == 0) {
    /* announce("Having my first thought--this is awful."); */
  }
  if (millis() >= nextThought) {
    nextThought += thoughtDelay;
    handleCommandChain();
    handleSerial();
  }
}
