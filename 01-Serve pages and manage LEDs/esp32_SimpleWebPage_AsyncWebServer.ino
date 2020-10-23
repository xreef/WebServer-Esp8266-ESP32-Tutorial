/*
 *  ESP32
 *  Simple web server an html page served
 *  URI served / and /inline 
 *
 *  by Mischianti Renzo <https://www.mischianti.org>
 *
 *  https://www.mischianti.org/category/tutorial/how-to-create-a-web-server-with-esp8266-and-esp32/
 *
 */
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

const char* ssid = "<YOUR-SSID>";
const char* password = "<YOUR-PASSWD>";

AsyncWebServer server(80);

void handleRoot(AsyncWebServerRequest *request) {
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf(temp, 400,

           "<html>\
			  <head>\
				<meta http-equiv='refresh' content='5'/>\
				<title>ESP8266 Demo</title>\
				<style>\
				  body { background-color: #00ffff4d; font-family: Arial, Helvetica, Sans-Serif; Color: blue; }\
				</style>\
			  </head>\
			  <body>\
           		<center>\
				<h1>Hi from your esp8266 HTML page!</h1>\
				<p>Uptime: %02d:%02d:%02d</p>\
				</center>\
				</body>\
			</html>",

           hr, min % 60, sec % 60
          );
  request->send(200, "text/html", temp);
}

void handleNotFound(AsyncWebServerRequest *request) {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += request->url();
  message += "\nMethod: ";
  message += (request->method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += request->args();
  message += "\n";

  for (uint8_t i = 0; i < request->args(); i++) {
    message += " " + request->argName(i) + ": " + request->arg(i) + "\n";
  }

  request->send(404, "text/plain", message);
}

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

  server.on("/", handleRoot);

  server.on("/inline", [](AsyncWebServerRequest *request) {
	  request->send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {

}