/*
 *  esp32 AsyncWebServer
 *  Simple web server that read from SPIFFS and
 *  stream on browser various type of file
 *  and manage gzip format also.
 *  Here I add a management of a token authentication
 *  with a custom login form, and relative logout.
 *
 * DHT12 library https://www.mischianti.org/2019/01/01/dht12-library-en/
 *
 * DHT12      ----- esp32
 * SCL        ----- 17
 * SDA        ----- 16
 *
 *  by Mischianti Renzo <https://www.mischianti.org>
 *
 *  https://www.mischianti.org/category/tutorial/how-to-create-a-web-server-with-esp8266-and-esp32/
 *
 */
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "mbedtls/md.h"
#include <SPIFFS.h>
#include <DHT12.h>
#include <ArduinoJson.h>

String sha1(String payloadStr){
	const char *payload = payloadStr.c_str();

	int size = 20;

	byte shaResult[size];

	mbedtls_md_context_t ctx;
	mbedtls_md_type_t md_type = MBEDTLS_MD_SHA1;

	const size_t payloadLength = strlen(payload);

	mbedtls_md_init(&ctx);
	mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
	mbedtls_md_starts(&ctx);
	mbedtls_md_update(&ctx, (const unsigned char *) payload, payloadLength);
	mbedtls_md_finish(&ctx, shaResult);
	mbedtls_md_free(&ctx);

    String hashStr = "";

    for(uint16_t i = 0; i < size; i++) {
        String hex = String(shaResult[i], HEX);
        if(hex.length() < 2) {
            hex = "0" + hex;
        }
        hashStr += hex;
    }

    return hashStr;
}

const char* ssid = "<YOUR-SSID>";
const char* password = "<YOUR-PASSWD>";

const char* www_username = "admin";
const char* www_password = "esp8266";

// Set dht12 i2c comunication on default Wire pin
DHT12 dht12;

AsyncWebServer httpServer(80);

void serverRouting();

void setup(void) {
	Serial.begin(115200);
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	Serial.println("");
	// Start sensor
	dht12.begin();

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
	if (SPIFFS.begin()) {
		Serial.println(F("done."));
	} else {
		Serial.println(F("fail."));
	}

	Serial.println("Set routing for http server!");
	serverRouting();
	httpServer.begin();
	Serial.println("HTTP server started");
}

void loop(void) {

}

String getContentType(String filename) {
	if (filename.endsWith(F(".htm"))) return F("text/html");
	else if (filename.endsWith(F(".html"))) return F("text/html");
	else if (filename.endsWith(F(".css"))) return F("text/css");
	else if (filename.endsWith(F(".js"))) return F("application/javascript");
	else if (filename.endsWith(F(".json"))) return F("application/json");
	else if (filename.endsWith(F(".png"))) return F("image/png");
	else if (filename.endsWith(F(".gif"))) return F("image/gif");
	else if (filename.endsWith(F(".jpg"))) return F("image/jpeg");
	else if (filename.endsWith(F(".jpeg"))) return F("image/jpeg");
	else if (filename.endsWith(F(".ico"))) return F("image/x-icon");
	else if (filename.endsWith(F(".xml"))) return F("text/xml");
	else if (filename.endsWith(F(".pdf"))) return F("application/x-pdf");
	else if (filename.endsWith(F(".zip"))) return F("application/x-zip");
	else if (filename.endsWith(F(".gz"))) return F("application/x-gzip");
	return F("text/plain");
}

bool handleFileRead(AsyncWebServerRequest *request, String path) {
	Serial.print(F("handleFileRead: "));
	Serial.println(path);

	if (!is_authenticated(request)) {
		Serial.println(F("Go on not login!"));
		path = "/login.html";
	} else {
		if (path.endsWith("/")) path += F("index.html"); // If a folder is requested, send the index file
	}
	String contentType = getContentType(path);             	// Get the MIME type
	String pathWithGz = path + F(".gz");
	  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){  	// If the file exists, either as a compressed archive, or normal
		bool gzipped = false;

	    if(SPIFFS.exists(pathWithGz)) {                        	// If there's a compressed version available
	      path += F(".gz");                                     // Use the compressed version
	      gzipped = true;
	    }
	    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, path, contentType);
	    if (gzipped){
	    	response->addHeader("Content-Encoding", "gzip");
	    }
	    Serial.print("Real file path: ");
	    Serial.println(path);

	    request->send(response);

	    return true;
	  }
	Serial.println(String(F("\tFile Not Found: ")) + path);
	return false;                     // If the file doesn't exist, return false
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

  request->send(200, "text/plain", message);
}

void handleLogin(AsyncWebServerRequest *request) {
	Serial.println("Handle login");
	String msg;
	if (request->hasHeader("Cookie")) {
		// Print cookies
		Serial.print("Found cookie: ");
		String cookie = request->header("Cookie");
		Serial.println(cookie);
	}

	if (request->hasArg("username") && request->hasArg("password")) {
		Serial.print("Found parameter: ");

		if (request->arg("username") == String(www_username) && request->arg("password") == String(www_password)) {
			AsyncWebServerResponse *response = request->beginResponse(301); //Sends 301 redirect

			response->addHeader("Location", "/");
			response->addHeader("Cache-Control", "no-cache");

			String token = sha1(String(www_username) + ":" + String(www_password) + ":" + request->client()->remoteIP().toString());
			Serial.print("Token: ");
			Serial.println(token);
			response->addHeader("Set-Cookie", "ESPSESSIONID=" + token);

			request->send(response);
			Serial.println("Log in Successful");
			return;
		}
		msg = "Wrong username/password! try again.";
		Serial.println("Log in Failed");
		AsyncWebServerResponse *response = request->beginResponse(301); //Sends 301 redirect

		response->addHeader("Location", "/login.html?msg=" + msg);
		response->addHeader("Cache-Control", "no-cache");
		request->send(response);
		return;
	}
}

/**
 * Manage logout (simply remove correct token and redirect to login form)
 */
void handleLogout(AsyncWebServerRequest *request) {
	Serial.println("Disconnection");
	AsyncWebServerResponse *response = request->beginResponse(301); //Sends 301 redirect

	response->addHeader("Location", "/login.html?msg=User disconnected");
	response->addHeader("Cache-Control", "no-cache");
	response->addHeader("Set-Cookie", "ESPSESSIONID=0");
	request->send(response);
	return;
}

/**
 * Retrieve temperature humidity realtime data
 */
void handleTemperatureHumidity(AsyncWebServerRequest *request){
	Serial.println("handleTemperatureHumidity");

	manageSecurity(request);

	Serial.println("handleTemperatureHumidity security pass!");

	const size_t capacity = 1024;
	DynamicJsonDocument doc(capacity);

//	doc["humidity"] = dht12.readHumidity();
//	doc["temp"] = dht12.readTemperature();

// If you don't have a DHT12 put only the library
// comment upper line and decomment this line
//	doc["humidity"] = random(10,80);
//	doc["temp"] = random(1000,3500)/100.;

	String buf;
	serializeJson(doc, buf);
	request->send(200, F("application/json"), buf);
}

//Check if header is present and correct
bool is_authenticated(AsyncWebServerRequest *request) {
	Serial.println("Enter is_authenticated");
	if (request->hasHeader("Cookie")) {
		Serial.print("Found cookie: ");
		String cookie = request->header("Cookie");
		Serial.println(cookie);

		String token = sha1(String(www_username) + ":" +
				String(www_password) + ":" +
				request->client()->remoteIP().toString());

		if (cookie.indexOf("ESPSESSIONID=" + token) != -1) {
			Serial.println("Authentication Successful");
			return true;
		}
	}
	Serial.println("Authentication Failed");
	return false;
}

void manageSecurity(AsyncWebServerRequest *request){
	if (!is_authenticated(request)) {
		request->send(401, F("application/json"), "{\"msg\": \"You must authenticate!\"}");
		return;
	}
}

void restEndPoint(){
	// External rest end point (out of authentication)
	httpServer.on("/login", HTTP_POST, handleLogin);
	httpServer.on("/logout", HTTP_GET, handleLogout);

	httpServer.on("/temperatureHumidity", HTTP_GET, handleTemperatureHumidity);
}

void serverRouting() {
	restEndPoint();

	// Manage Web Server
	Serial.println(F("Go on not found!"));
	httpServer.onNotFound([](AsyncWebServerRequest *request) {               // If the client requests any URI
				Serial.println(F("On not found"));
			    if (!handleFileRead(request, request->url())){                  // send it if it exists
			    	handleNotFound(request); // otherwise, respond with a 404 (Not Found) error
				}
			});

	Serial.println(F("Set cache!"));
	// Serve a file with no cache so every tile It's downloaded
	httpServer.serveStatic("/configuration.json", SPIFFS, "/configuration.json", "no-cache, no-store, must-revalidate");
	// Server all other page with long cache so browser chaching they
	httpServer.serveStatic("/", SPIFFS, "/", "max-age=31536000");
}

