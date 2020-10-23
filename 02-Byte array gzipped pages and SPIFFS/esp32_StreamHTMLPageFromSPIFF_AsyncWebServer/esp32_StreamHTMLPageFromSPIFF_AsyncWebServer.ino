/*
 *  ESP32
 *  Simple web server
 *  an html page stored on SPIFFS
 *  stream on browser
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

const char* ssid = "<your-ssid>";
const char* password = "<your-passwd>";

AsyncWebServer server(80);

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

bool loadFromSPIFFS(AsyncWebServerRequest *request, String path) {
  String dataType = "text/html";

  Serial.print("Requested page -> ");
  Serial.println(path);
  if (SPIFFS.exists(path)){
	  File dataFile = SPIFFS.open(path, "r");
	  if (!dataFile) {
		  handleNotFound(request);
		  return false;
	  }

	    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, path, dataType);
	    Serial.print("Real file path: ");
	    Serial.println(path);

	    request->send(response);


	  dataFile.close();
  }else{
	  handleNotFound(request);
	  return false;
  }
  return true;
}


void handleRoot(AsyncWebServerRequest *request) {
	loadFromSPIFFS(request, "/index.html");
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

  server.on("/inline", [](AsyncWebServerRequest *request) {
	  request->send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {

}