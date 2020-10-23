/*
 *  WeMos D1 mini (esp8266)
 *  Simple web server an html page served with 2 end point
 * that change status of builtin LED
 *  URI served / /ledon /ledoff 
 *  by Mischianti Renzo <https://www.mischianti.org>
 *
 *  https://www.mischianti.org/category/tutorial/how-to-create-a-web-server-with-esp8266-and-esp32/
 *
 */
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define LED_PIN LED_BUILTIN
#define LED_ON LOW
#define LED_OFF HIGH

const char* ssid = "<YOUR-SSID>";
const char* password = "<YOUR-PASSWD>";

ESP8266WebServer server(80);

bool ledStatus = false;

String handleRoot(bool ledStatus = false) {
	String ptr =
			"<html>\
			  <head>\
				<title>ESP8266 Demo</title>\
				<style>\
				  body { background-color: #00ffff4d; font-family: Arial, Helvetica, Sans-Serif; Color: blue; }\
				</style>\
			  </head>\
			  <body>\
           		<center>\
				<h1>Hi from your esp8266 HTML page!</h1>\
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

void handleRootDefault(){
	ledStatus = false;
	digitalWrite(LED_PIN, LED_OFF);
	Serial.println("LED Status: OFF");

	server.send(200, "text/html", handleRoot(ledStatus));
}

void handleLedOn() {
  ledStatus = true;
  Serial.println("LED Status: ON");
  digitalWrite(LED_PIN, LED_ON);
  server.send(200, "text/html", handleRoot(ledStatus));
}

void handleLedOff() {
  ledStatus = LOW;
  Serial.println("LED Status: OFF");
  digitalWrite(LED_PIN, LED_OFF);
  server.send(200, "text/html", handleRoot(ledStatus));
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
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
  server.handleClient();
}