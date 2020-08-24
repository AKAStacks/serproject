// IR Project
// This code is without warranty and public domain

#define _IR_ENABLE_DEFAULT_ false
#define SEND_SAMSUNG true

#include <PolledTimeout.h>
#include <Wire.h>
#include <Ticker.h>
Ticker ticker;
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

String inputString = "";
bool stringComplete = false;

bool inCommandChain = false;
enum BrightAdjust = { up, down, none };
BrightAdjust brightAdjust = none;
int arrayLength = 0;
int currentCommandStep = 0;

void tick() {
  digitalWrite(LED, !digitalRead(LED));
}

void toggleTV() {
   irsend.sendSAMSUNG(SamsungPowerToggle);
   // Serial.println("Sending toggle IR signal.");
}

void handleSerial() {
  if (Serial.available()) {
    digitalWrite(LED_BUILTIN, HIGH);
    inputString = Serial.readString();
    stringComplete = true;
  }
  if (stringComplete) {
    inputString.trim();
    if (inputString == "toggle" || inputString == "ߗ") {
      toggleTV();
    } else {
      // Serial.println("Unrecognized command: " + inputString);
      while (Serial.available() > 0) {
        Serial.read();
      }
   }
    digitalWrite(LED_BUILTIN, LOW);
    stringComplete = false;
    inputString = "";
  }
}

void doNextCommand(direction) {
  int arrayLength = 0;
  switch(direction) {
    case up:
      arrayLength = sizeof(dayLight)/sizeof(dayLight[0]);
      break;
    case down:
      arrayLength = sizeof(nightLight)/sizeof(nightLight[0]);
      break;
  }

  if
}

void handleCommandChain() {
  if (inCommandChain) {
    if (arrayLength == 0) {
      switch(brightAdjust) {
        case up:
          arrayLength = sizeof(dayLight)/sizeof(dayLight[0]);
          break;
        case down:
          arrayLength = sizeof(nightLight)/sizeof(nightLight[0]);
          break;
      }
    }

    if (currentCommandStep >= arrayLength) {
      inCommandChain = false;

    }

  }
}

void setup() {
  Serial.begin(115200);

  pinMode(IRLED, OUTPUT);

  inputString.reserve(200);

  irsend.begin();
}

void loop() {
  using periodic = esp8266::polledTimeout::periodicMs;
  static periodic nextPing(30000);

  handleSerial();

}
