// IR Project
// This code is without warranty and public domain

#include "credentials.h"

#ifndef STASSID
#define STASSID WIFI_SSID
#define STAPSK WIFI_PASSWD
#endif

#include <ESP8266WebServer.h>
#include <Uri.h>

#include <WiFiClient.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 13 // ESP32 DOES NOT DEFINE LED_BUILTIN
#endif

#define DEBUG 1

int LED = LED_BUILTIN;
ESP8266WebServer server(80);

const char* ssid = STASSID;
const char* password = STAPSK;

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

void sendSer(String command) {
  // Define static bytes
  byte message[7];
  message[0] = 0x08;
  message[1] = 0x22;

  // Put correct bytes in array
  if (command == "powerToggle") {
    // format: fixed, fixed, cmd1, cmd2, cmd3, value, checksum
    message[2] = 0x00;
    message[3] = 0x00;
    message[4] = 0x00;
    message[5] = 0x00;
  } else if (command == "volumeUp") {
    message[2] = 0x01;
    message[3] = 0x00;
    message[4] = 0x01;
    message[5] = 0x00;
  } else if (command == "volumeDown") {
    message[2] = 0x01;
    message[3] = 0x00;
    message[4] = 0x02;
    message[5] = 0x00;
  } else if (command == "muteToggle") {
    message[2] = 0x04;
    message[3] = 0x00;
    message[4] = 0x00;
    message[5] = 0x00;
  }
  // Add checksum at end
  message[6] = calcCheckSum(message);

  if (DEBUG == 1) {
    for (int i = 0; i < 7; i++) {
      printf("Byte %d sent: %X (%d)\n", i, message[i], message[i]);
    }
  }
  Serial1.write(message, sizeof(message));
  Serial1.flush();
  flashLED();
}

byte calcCheckSum(byte message[]) {
  byte checkSum = 0x00;
  for (int i = 0; i < 6; i++) {
    checkSum += message[i];
  }
  checkSum = ~checkSum;
  checkSum = (checkSum + 1) & 0xff;
  return checkSum;
}

void tvToggle() {
  /* irsend.sendSAMSUNG(SamsungPowerToggle);
  announce("OK, turning the TV off..."); */
  sendSer("powerToggle");
}

void tvBrightUp() {
  announce("OK, setting TV to day mode.");
}

void tvBrightDown() {
  announce("OK, setting TV to night mode.");
}

void tvExit() {
}

void tvSource() {
}

void tvMute() {
  sendSer("muteToggle");
}

void tvVolUp() {
  sendSer("volumeUp");
}

void tvVolDown() {
  sendSer("volumeDown");
}

void handleRoot() {
/* 1 = toggle TV
   2 = daylight
   3 = nightlight
   4 = exit 
   5 = source
   6 = mute
   7 = volup
   8 = voldown */
  server.send(200, "text/html",
  "<html>\
    <head>\
      <style>\
        * button {\
          width: 100%;\
          height: 100%;\
          font-size: 5em;\
        }\
      </style>\
    </head>\
    <body style='background: black; overflow: hidden;'>\
        <div style='width: 100vw; height: 100vh; display: grid; place-items: center; color: white;'>\
       <button type='button' onclick='doLED(1);'>Toggle TV</button>\
        <button type='button' onclick='doLED(2);'>Daylight</button>\
        <button type='button' onclick='doLED(3);'>Nightlight</button>\
        <button type='button' onclick='doLED(4);'>Cancel</button>\
        <button type='button' onclick='doLED(5);'>Source</button>\
        <button type='button' onclick='doLED(6);'>Mute</button>\
        <button type='button' onclick='doLED(7);'>Vol Up</button>\
        <button type='button' onclick='doLED(8);'>Vol Down</button>\
        <script>\
            var ledState = 0;\
            function doLED(doWhat) {\
              let xhttp = new XMLHttpRequest();\
              xhttp.timeout = 500;\
              xhttp.onreadystatechange = function() {\
                if (this.readyState == 4 && this.status == 200) {}};\
              if (doWhat == 1) {\
                xhttp.open('POST', 'toggle.txt', true);\
              } else if (doWhat == 2) {\
                xhttp.open('POST', 'daylight.txt', true);\
              } else if (doWhat == 3) {\
                xhttp.open('POST', 'nightlight.txt', true);\
              } else if (doWhat == 4) {\
                xhttp.open('POST', 'exit.txt', true);\
              } else if (doWhat == 5) {\
                xhttp.open('POST', 'source.txt', true);\
              } else if (doWhat == 6) {\
                xhttp.open('POST', 'mute.txt', true);\
              } else if (doWhat == 4) {\
                xhttp.open('POST', 'exit.txt', true);\
              } else if (doWhat == 7) {\
                xhttp.open('POST', 'volup.txt', true);\
              } else if (doWhat == 8) {\
                xhttp.open('POST', 'voldown.txt', true);\
              }\
              xhttp.send();\
            };\
        </script>\
      </div>\
     </body>\
   </html>");
}

void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);

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
  server.on("/toggle.txt", tvToggle);
  server.on("/daylight.txt", tvBrightUp);
  server.on("/nightlight.txt", tvBrightDown);
  server.on("/exit.txt", tvExit);
  server.on("/source.txt", tvSource);
  server.on("/mute.txt", tvMute);
  server.on("/volup.txt", tvVolUp);
  server.on("/voldown.txt", tvVolDown);
  server.begin();

  Serial1.begin(9600);

  digitalWrite(LED_BUILTIN, HIGH);
  announce("Waking up... ahh...");
}

void loop() {
  server.handleClient();
}
