#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Nome da rede que o ESP32 criará para configuração
const char* ap_ssid = "TermoLog-Config";

WebServer server(80);

// Pinos dos sensores conforme esquema anterior
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define ONE_WIRE_BUS 15
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Interface de Configuração HTML
const char CONFIG_PAGE[] = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta charset='UTF-8'>
  <meta name='viewport' content='width=device-width, initial-scale=1'>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; padding: 20px; background-color: #f4f4f4; }
    .box { background: white; padding: 20px; border-radius: 10px; box-shadow: 0 2px 5px rgba(0,0,0,0.2); display: inline-block; }
    input { width: 90%; padding: 10px; margin: 10px 0; border: 1px solid #ccc; border-radius: 5px; }
    button { padding: 10px 20px; background: #2196f3; color: white; border: none; border-radius: 5px; cursor: pointer; font-weight: bold; }
  </style>
</head>
<body>
  <div class='box'>
    <h2>Configurar Wi-Fi TermoLog</h2>
    <form action='/save' method='POST'>
      <input type='text' name='ssid' placeholder='Nome da sua Rede' required><br>
      <input type='password' name='pass' placeholder='Senha da sua Rede' required><br>
      <button type='submit' onclick='return confirm("Confirma que você está na mesma rede que inseriu acima?")'>CONECTAR E SALVAR</button>
    </form>
  </div>
</body>
</html>
)=====";

void handleRoot() {
  server.send(200, "text/html", CONFIG_PAGE);
}

void handleSave() {
  if (server.hasArg("ssid") && server.hasArg("pass")) {
    String s = server.arg("ssid");
    String p = server.arg("pass");
    
    server.send(200, "text/html", "<h1>Salvando...</h1><p>O ESP32 vai reiniciar para tentar conectar em: " + s + "</p>");
    delay(2000);
    
    // Inicia a conexão com os novos dados
    WiFi.begin(s.c_str(), p.c_str());
    ESP.restart(); 
  }
}

void handleData() {
  float h = dht.readHumidity();
  float t1 = dht.readTemperature();
  sensors.requestTemperatures();
  float t2 = sensors.getTempCByIndex(0);
  
  String json = "{\"temp\":\"" + String(t1) + "\", \"umid\":\"" + String(h) + "\", \"temp_extra\":\"" + String(t2) + "\"}";
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  sensors.begin();

  WiFi.mode(WIFI_AP_STA); // Habilita ambos os modos
  WiFi.begin(); // Tenta usar as últimas credenciais salvas internamente

  Serial.println("Tentando conectar ao Wi-Fi salvo...");
  unsigned long startAttempt = millis();
  
  // Tenta por 15 segundos antes de desistir e abrir o Ponto de Acesso
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 15000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConectado com sucesso!");
    MDNS.begin("termolog");
  } else {
    Serial.println("\nFalha na conexão. Abrindo rede de configuração...");
    WiFi.softAP(ap_ssid); // Cria a rede "TermoLog-Config" sem senha
    Serial.println("Conecte-se em 'TermoLog-Config' e acesse 192.168.4.1");
  }

  server.on("/", handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();
}