/*
 *  ESP32
 *  Simple web server an html page served with 2 end point
 * that change status of builtin LED
 *  URI served / /ledon /ledoff 
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

#define LED_PIN LED_BUILTIN
#define LED_ON LOW
#define LED_OFF HIGH

const char* ssid = "<YOUR-SSID>";
const char* password = "<YOUR-PASSWD>";

AsyncWebServer server(80);


bool ledStatus = false;

String handleRoot(bool ledStatus = false) {
	String ptr =
			"<html>\
			  <head>\
				<title>ESP8266 Builtin led control</title>\
				<style>\
				  body { background-color: #00ffff4d; font-family: Arial, Helvetica, Sans-Serif; Color: blue; }\
				</style>\
			  </head>\
			  <body>\
           		<center>\
				<h2>ESP8266 Builtin led control!</h2>\
				";

	if (ledStatus) {
		ptr +=
				"<p>LED: ON</p><a class=\"button button-off\" href=\"/ledoff\">OFF</a>\n";
	} else {
		ptr +=
				"<p>LED: OFF</p><a class=\"button button-on\" href=\"/ledon\">ON</a>\n";
	}

	ptr += "</center>\
			</body>\
			</html>";

	return ptr;
}

void handleRootDefault(AsyncWebServerRequest *request){
	ledStatus = false;
	digitalWrite(LED_PIN, LED_OFF);
	Serial.println("LED Status: OFF");

	request->send(200, "text/html", handleRoot(ledStatus));
}

void handleLedOn(AsyncWebServerRequest *request) {
  ledStatus = true;
  Serial.println("LED Status: ON");
  digitalWrite(LED_PIN, LED_ON);
  request->send(200, "text/html", handleRoot(ledStatus));
}

void handleLedOff(AsyncWebServerRequest *request) {
  ledStatus = LOW;
  Serial.println("LED Status: OFF");
  digitalWrite(LED_PIN, LED_OFF);
  request->send(200, "text/html", handleRoot(ledStatus));
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

  pinMode(LED_PIN, OUTPUT);

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRootDefault);

  server.on("/ledon", handleLedOn);
  server.on("/ledoff", handleLedOff);

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {

}