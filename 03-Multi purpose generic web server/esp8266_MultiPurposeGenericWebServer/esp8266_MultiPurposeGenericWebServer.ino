/*
 *  WeMos D1 mini (esp8266)
 *  Simple web server that read from SPIFFS and
 *  stream on browser various type of file
 *  and manage gzip format also
 *
 *  by Mischianti Renzo <https://www.mischianti.org>
 *
 *  https://www.mischianti.org/category/tutorial/how-to-create-a-web-server-with-esp8266-and-esp32/
 *
 */
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>

const char* ssid = "<YOUR-SSID>";
const char* password = "<YOUR-PASSWD>";

ESP8266WebServer httpServer(80);


bool loadFromSPIFFS(String path, String dataType) {
  Serial.print("Requested page -> ");
  Serial.println(path);
  if (SPIFFS.exists(path)){
	  File dataFile = SPIFFS.open(path, "r");
	  if (!dataFile) {
		  handleNotFound();
		  return false;
	  }

	  if (httpServer.streamFile(dataFile, dataType) != dataFile.size()) {
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

void serverRouting();

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

  Serial.println("Set routing for http server!");
  serverRouting();
  httpServer.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
	httpServer.handleClient();
}

String getContentType(String filename){
  if(filename.endsWith(F(".htm"))) 			return F("text/html");
  else if(filename.endsWith(F(".html"))) 	return F("text/html");
  else if(filename.endsWith(F(".css"))) 	return F("text/css");
  else if(filename.endsWith(F(".js"))) 		return F("application/javascript");
  else if(filename.endsWith(F(".json"))) 	return F("application/json");
  else if(filename.endsWith(F(".png"))) 	return F("image/png");
  else if(filename.endsWith(F(".gif"))) 	return F("image/gif");
  else if(filename.endsWith(F(".jpg"))) 	return F("image/jpeg");
  else if(filename.endsWith(F(".jpeg"))) 	return F("image/jpeg");
  else if(filename.endsWith(F(".ico"))) 	return F("image/x-icon");
  else if(filename.endsWith(F(".xml"))) 	return F("text/xml");
  else if(filename.endsWith(F(".pdf"))) 	return F("application/x-pdf");
  else if(filename.endsWith(F(".zip"))) 	return F("application/x-zip");
  else if(filename.endsWith(F(".gz"))) 		return F("application/x-gzip");
  return F("text/plain");
}

bool handleFileRead(String path){
  Serial.print(F("handleFileRead: "));
  Serial.println(path);

  if(path.endsWith("/")) path += F("index.html");           // If a folder is requested, send the index file
  String contentType = getContentType(path);             	// Get the MIME type
  String pathWithGz = path + F(".gz");
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){  	// If the file exists, either as a compressed archive, or normal
    if(SPIFFS.exists(pathWithGz))                          	// If there's a compressed version available
      path += F(".gz");                                     // Use the compressed version
    fs::File file = SPIFFS.open(path, "r");                 // Open the file
    size_t sent = httpServer.streamFile(file, contentType); // Send it to the client
    file.close();                                          	// Close the file again
    Serial.println(String(F("\tSent file: ")) + path + String(F(" of size ")) + sent);
    return true;
  }
  Serial.println(String(F("\tFile Not Found: ")) + path);
  return false;                                          	// If the file doesn't exist, return false
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += httpServer.uri();
  message += "\nMethod: ";
  message += (httpServer.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += httpServer.args();
  message += "\n";

  for (uint8_t i = 0; i < httpServer.args(); i++) {
    message += " " + httpServer.argName(i) + ": " + httpServer.arg(i) + "\n";
  }

  httpServer.send(404, "text/plain", message);
}

void serverRouting() {
	  httpServer.onNotFound([]() {                              // If the client requests any URI
		  Serial.println(F("On not found"));
	    if (!handleFileRead(httpServer.uri())){                  // send it if it exists
	    	handleNotFound(); // otherwise, respond with a 404 (Not Found) error
	    }
	  });

	  Serial.println(F("Set cache!"));
	  // Serve a file with no cache so every tile It's downloaded
	  httpServer.serveStatic("/configuration.json", SPIFFS, "/configuration.json","no-cache, no-store, must-revalidate");
	  // Server all other page with long cache so browser chaching they
	  httpServer.serveStatic("/", SPIFFS, "/","max-age=31536000");


}
