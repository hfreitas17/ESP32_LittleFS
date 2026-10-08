#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

// Credenciais da rede Wi-Fi
const char* ssid = "2G_Wifi_Not_Found";
const char* password = "85DC-MXVK@85";

// Define o pino do LED (geralmente pino 2 no ESP32 NodeMCU)
const int ledPin = 2;
bool ledState = false;

// Cria o servidor web na porta 80
AsyncWebServer server(80);

// Funcao que substitui os placeholders (%STATUS_LED%) no HTML pelo valor real
String processor(const String& var) {
    if (var == "STATUS_LED") {
        return ledState ? "LIGADO" : "DESLIGADO";
    }
    return String();
}



void setup() {
  Serial.begin(115200);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);

    // 1. Inicializa o LittleFS
    if (!LittleFS.begin()) {
        Serial.println("Erro ao montar o LittleFS!");
        return;
    }

    // 2. Conecta ao Wi-Fi
    WiFi.begin(ssid, password);
    Serial.print("Conectando ao Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConectado com sucesso!");
    Serial.print("Endereço IP do ESP32: ");
    Serial.println(WiFi.localIP());

    // 1. Rota principal: Agora serve apenas o arquivo HTML puro (sem o processor lento)
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/index.html", "text/html");
    });
    
     // 1.1: Entrega o arquivo CSS com o cabeçalho correto ("text/css")
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/style.css", "text/css");
    });

     // 2. Rota que o JavaScript consulta para saber o estado atual ao abrir a página
    server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", ledState ? "LIGADO" : "DESLIGADO");
    });

    // 3. Rota para LIGAR o LED (retorna o novo status para o Fetch API)
    server.on("/led/on", HTTP_GET, [](AsyncWebServerRequest *request){
        ledState = true;
        digitalWrite(ledPin, HIGH);
        request->send(200, "text/plain", "LIGADO"); 
    });

    // 4. Rota para DESLIGAR o LED (retorna o novo status para o Fetch API)
    server.on("/led/off", HTTP_GET, [](AsyncWebServerRequest *request){
        ledState = false;
        digitalWrite(ledPin, LOW);
        request->send(200, "text/plain", "DESLIGADO");
    });

    // Inicializa o servidor
    server.begin();
  
}

void loop() {
   // Como o ESPAsyncWebServer e assincrono, nao precisamos colocar nada aqui!
}

