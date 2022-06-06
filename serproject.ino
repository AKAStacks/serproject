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

#define DEBUG 0

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
    message[2] = 0x02;
    message[3] = 0x00;
    message[4] = 0x00;
    message[5] = 0x00;
  } else if (command == "dayLight") {
    message[2] = 0x0b;
    message[3] = 0x00;
    message[4] = 0x00;
    message[5] = 0x00;
  } else if (command == "nightLight") {
    message[2] = 0x0b;
    message[3] = 0x00;
    message[4] = 0x00;
    message[5] = 0x02;
  } else if (command == "sourcePC") {
    message[2] = 0x0a;
    message[3] = 0x00;
    message[4] = 0x05;
    message[5] = 0x00;    
  } else if (command == "sourceRoku") {
    message[2] = 0x0a;
    message[3] = 0x00;
    message[4] = 0x05;
    message[5] = 0x01;      
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

  server.send(200, "text/plain", command);
  
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
  sendSer("dayLight");
  announce("OK, setting TV to day mode.");
}

void tvBrightDown() {
  sendSer("nightLight");
  announce("OK, setting TV to night mode.");
}

void tvSourcePC() {
  sendSer("sourcePC");
}

void tvSourceRoku() {
  sendSer("sourceRoku");
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
         width: 95%;\
         height: 95%;\
         font-size: 5em;\
         margin: .2em;\
         background-color: 222222;\
         border-color: 111111;\
         color: cccccc;\
         border-radius: .2em;\
        }\
        @keyframes buttonPush {\
          from {background-color: limegreen;}\
          to {background-color: 222222;}\
        }\
        .buttonClicked {\
          animation-name: buttonPush;\
          animation-duration: 2s;\
        }\
        .reset {\
          animation-name: none;\
          animation-duration: 2s;\
        }\
        #power {\
          grid-column-start: 1;\
          grid-column-end: 3;\
          grid-row-start: 1;\
          grid-row-end: 1;\
        }\
        #mute {\
          grid-column-start: 1;\
          grid-column-end: 3;\
          grid-row-start: 4;\
          grid-row-end: 4;\
        }\
        #daylight {\
          grid-column-start: 1;\
          grid-column-end: 1;\
          grid-row-start: 2;\
          grid-row-end: 2;\
        }\
        #nightlight {\
          grid-column-start: 1;\
          grid-column-end: 1;\
          grid-row-start: 3;\
          grid-row-end: 3;\
        }\
        #pc {\
          grid-column-start: 2;\
          grid-column-end: 2;\
          grid-row-start: 2;\
          grid-row-end: 2;\
        }\
        #roku {\
          grid-column-start: 2;\
          grid-column-end: 2;\
          grid-row-start: 3;\
          grid-row-end: 3;\
        }\
        #volup {\
         grid-row-start: 5;\
         grid-row-end: 5;\
         grid-column-start: 1;\
         grid-column-end: 3;\
        }\
        #voldown {\
         grid-row-start: 6;\
         grid-row-end: 6;\
         grid-column-start: 1;\
         grid-column-end: 3;\
        }\
      </style>\
    </head>\
    <body style='background: black; overflow: scroll;'>\
        <div style='width: 100vw; height: 100vh; display: grid; place-items: center; grid-template-columns: 50% 50%; grid-template-rows: auto auto auto auto auto auto; color: white;'>\
        <button id='power' type='button' onclick='doLED(event, 1);'>Toggle TV</button>\
        <button id='daylight' type='button' onclick='doLED(event, 2);'>Daylight</button>\
        <button id='nightlight' type='button' onclick='doLED(event, 3);'>Nightlight</button>\
        <button id='pc' type='button' onclick='doLED(event, 4);'>PC</button>\
        <button id='roku' type='button' onclick='doLED(event, 5);'>Roku</button>\
        <button id='mute' type='button' onclick='doLED(event, 6);'>Mute</button>\
        <button id='volup' type='button' onclick='doLED(event, 7);'>Vol Up</button>\
        <button id='voldown' type='button' onclick='doLED(event, 8);'>Vol Down</button>\
        <script>\
            var ledState = 0;\
            function doLED(event, doWhat) {\
              event.target.className = 'reset';\
              event.target.offsetWidth;\
              let xhttp = new XMLHttpRequest();\
              xhttp.onreadystatechange = function() {\
                if (this.readyState == 4 && this.status == 200) {}};\
              if (doWhat == 1) {\
                xhttp.open('POST', 'toggle.txt', true);\
              } else if (doWhat == 2) {\
                xhttp.open('POST', 'daylight.txt', true);\
              } else if (doWhat == 3) {\
                xhttp.open('POST', 'nightlight.txt', true);\
              } else if (doWhat == 4) {\
                xhttp.open('POST', 'sourcepc.txt', true);\
              } else if (doWhat == 5) {\
                xhttp.open('POST', 'sourceroku.txt', true);\
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
              event.target.className = 'buttonClicked';\
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
  server.on("/sourcepc.txt", tvSourcePC);
  server.on("/sourceroku.txt", tvSourceRoku);
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
