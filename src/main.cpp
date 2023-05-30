#include <stdio.h>
#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

#define WIFI_RETRY_DELAY 500
#define MAX_WIFI_INIT_RETRY 50

#define HTTP_REST_PORT 80
const char* wifi_ssid = "xxxxxx";
const char* wifi_passwd = "xxxxxxx";

ESP8266WebServer http_rest_server(HTTP_REST_PORT);

int led = D4;

////////////////////
//    Ini_Wifi    
/////////////////////

int init_wifi(){
int retries = 0;
  
  Serial.println("Connecting to WiFi ...");
  // set Wifi Mode
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_passwd);
  // check the status of WiFi connection to be WL_CONNECTED
  while ((WiFi.status() != WL_CONNECTED) && (retries < MAX_WIFI_INIT_RETRY)) {
    retries++;
    Serial.println(WiFi.localIP());
    delay(WIFI_RETRY_DELAY);
    Serial.print(".");
    // Dots will print to show still connecting...
  }
  return WiFi.status(); // return the WiFi connection status
}

void get_speed() {
    StaticJsonDocument<200> jsonObj;
    char JSONmessageBuffer[200];

    jsonObj["id"] = "speed";
    serializeJsonPretty(jsonObj,JSONmessageBuffer,sizeof(JSONmessageBuffer));
    http_rest_server.send(200, "application/json", JSONmessageBuffer);
}

void config_rest_server_routing() {
  http_rest_server.on("/", HTTP_GET, []() {
    http_rest_server.send(200, "text/html",  "Welcome to REST Web Server");
  });

   http_rest_server.on("/getspeed", HTTP_GET, get_speed);
}



////////////////////
//    setup
////////////////////

void setup() {
  pinMode(led,OUTPUT);
  digitalWrite(led,LOW);
  Serial.begin(9600);
  Serial.println("MCU Program Started...");
  if (init_wifi() == WL_CONNECTED) {
    Serial.println("WIFI Connected");
    Serial.print(wifi_ssid);
    Serial.print("--- IP: ");
    Serial.println(WiFi.localIP());
    digitalWrite(led,HIGH);
    delay(5000);
  }
  else {
    Serial.print("Error connecting to: ");
    Serial.println(wifi_ssid);
  }
  config_rest_server_routing();

  http_rest_server.begin();
  Serial.println("HTTP REST Server Started");
}

////////////////////
//    loop
////////////////////

void loop() {
  http_rest_server.handleClient();
}