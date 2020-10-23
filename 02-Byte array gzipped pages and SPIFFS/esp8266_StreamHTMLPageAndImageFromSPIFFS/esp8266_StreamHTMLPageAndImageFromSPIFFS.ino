/*
 *  WeMos D1 mini (esp8266)
 *  Simple web server that read from SPIFFS and
 *  stream on browser (page and image)
 *  
 *  by Mischianti Renzo <https://www.mischianti.org>
 *
 *  https://www.mischianti.org/category/tutorial/how-to-create-a-web-server-with-esp8266-and-esp32/
 *
 */
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>

const char* ssid = "<your-ssid>";
const char* password = "<your-passwd>";

ESP8266WebServer server(80);

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

bool loadFromSPIFFS(String path, String dataType) {
  Serial.print("Requested page -> ");
  Serial.println(path);
  if (SPIFFS.exists(path)){
	  File dataFile = SPIFFS.open(path, "r");
	  if (!dataFile) {
		  handleNotFound();
		  return false;
	  }

	  if (server.streamFile(dataFile, dataType) != dataFile.size()) {
	    Serial.println("Sent less data than expected!");
	  }else{
		  Serial.println("Page served!");
	  }

	  dataFile.close();
  }else{
	  handleNotFound();
	  return false;
  }
  return true;
}


void handleRoot() {
	loadFromSPIFFS("/index.html", "text/html");
}
void handleLogo() {
	loadFromSPIFFS("/logo256.jpg", "image/jpeg");
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

  Serial.print(F("Inizializing FS..."));
  if (SPIFFS.begin()){
	Serial.println(F("done."));
  }else{
	Serial.println(F("fail."));
  }

  server.on("/", handleRoot);
  server.on("/logo256.jpg", handleLogo);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}