/*
 *  ESP32
 *  Simple web server
 *  URI served / and /inline
 *
 *  by Mischianti Renzo <https://www.mischianti.org>
 *
 *  https://www.mischianti.org/
 *
 */
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

const char* ssid = "<YOUR-SSID>";
const char* password = "<YOUR-PASSWD>";

AsyncWebServer server(80);

void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", [](AsyncWebServerRequest *request) {
	  request->send(200, "text/plain", "Hello from esp8266 server web!");
  });

  server.on("/inline", [](AsyncWebServerRequest *request) {
	  request->send(200, "text/plain", "this works as well");
  });

  server.onNotFound([](AsyncWebServerRequest *request) {
	  request->send(404, "text/plain", "Uri not found "+request->url());
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
//  server.handleClient();
}