#include <WiFi.h>
#include <WebServer.h>
#include "DHT.h"

// --- CONFIGURAÇÃO WI-FI ---
const char* ssid = "CORTEZ 2";
const char* password = "46536410";

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

WebServer server(80);

void handleData() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  // Cria um JSON para o App ler facilmente
  String json = "{\"temp\":\"" + String(t) + "\", \"umid\":\"" + String(h) + "\"}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  
  Serial.println("\nConectado! IP do ESP32:");
  Serial.println(WiFi.localIP()); // ANOTE ESTE IP

  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();
}