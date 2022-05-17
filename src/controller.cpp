#include <Arduino.h>

#include "controller.h"
#include "main.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>
#include "LittleFS.h"


#include <ESP8266mDNS.h>
// #include <WiFiUdp.h>
// #include <WiFiClient.h>
// #include <WiFiAP.h>

// Set these to your desired credentials.
const char *ssid = "yourAP2";
const char *password = "1234";

// String State;
// String temperatureC;

// String State ="70";
// String temperatureC = "45";

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

AsyncWebServer server(80);

// const char* PARAM_INPUT_1 = "proportional";
// const char* PARAM_INPUT_2 = "integral";
// const char* PARAM_INPUT_3 = "derivative";
const char* PARAM_INPUT_4 = "temp";

const char* saveTemp = "/temp.txt";

// Replaces placeholder with temperature state value
String processor(const String& var){
  // Serial.println(var);
  if(var == "SET"){
    return State;
  }
  return String();
}

// Read File from SPIFFS
String readFile( const char * path){
  Serial.printf("Reading file: %s\r\n", path);

  File file = LittleFS.open(path,"r");
  if(!file || file.isDirectory()){
    Serial.println("- failed to open file for reading");
    return String();
  }
  
  String fileContent;
  while(file.available()){
    fileContent = file.readStringUntil('\n');
    break;     
  }
  file.close();
  return fileContent;
}

// Write file to SPIFFS
void writeFile(const char * path, const char * message){
  Serial.printf("Writing file: %s\r\n", path);

  File file = LittleFS.open(path, "w");
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- frite failed");
  }
}

void controllersetup() {
    if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  State = readFile(saveTemp);
  set_temperature =State.toFloat();
  Serial.println(State);
Serial.println(temperatureC);

  // Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  // WiFi.setmode(wifi.SOFTAP);
  WiFi.softAP("ESPsoftAP_01", password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  Serial.println("Server started");
  if(!MDNS.begin("esp8266")) {
     Serial.println("Error encountered while starting mDNS");
     return;
  }

   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/style.css", "text/css");
  });

  server.on("/jquery-3.6.0.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/jquery-3.6.0.min.js", "text/javascript");
  });

     server.on("/tempmanager", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/tempmanager.html", String(), false, processor);
  });

    server.on("/temperaturec", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", temperatureC.c_str());
  });

      server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST temperature value
          if (p->name() == PARAM_INPUT_4) {
            State = p->value().c_str();
            Serial.print("temp set to: ");
            Serial.println(p->value().c_str());
            // Write file to save value
            // writeFile(LittleFS, gatewayPath, gateway.c_str());
            set_temperature =State.toFloat();
          }
          if (p->name() == "save") {
            State = p->value().c_str();
            Serial.print("temp saved to: ");
            Serial.println(p->value().c_str());
            // Write file to save value saveTemp
            writeFile(saveTemp, State.c_str());
           set_temperature =State.toFloat();
          }
          // Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      request->send(LittleFS, "/index.html", String(), false, processor);
      // request->send(200);
    //   request->send(200, "text/html", index_html);
    // newRequest = true;
      // request->send(200, "text/plain", "Done. ESP will restart, connect to your router and go to IP address: ");
    });

  server.begin();
}

// void loop() {
// }