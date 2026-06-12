#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "ESP32C3_LDR_AP";
const char* password = "12345678";

AsyncWebServer server(80);

const int ldrPin = 1;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head><meta charset='utf-8'>
<title>ESP32-C3 - Monitor LDR</title>
<meta name='viewport' content='width=device-width, initial-scale=1'>
<style>
body { font-family: 'Segoe UI', Arial, sans-serif; text-align: center; margin-top: 50px; background-color: #f0f2f5; color: #333; }
.card { background: white; max-width: 400px; margin: 0 auto; padding: 30px; border-radius: 15px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }
h1 { color: #007bff; font-size: 24px; }
.leitura { font-size: 48px; font-weight: bold; color: #28a745; margin: 20px 0; }
p { color: #666; font-size: 14px; }
</style>
<script>
setInterval(function() {
  fetch('/data').then(response => response.text()).then(texto => {
    document.getElementById('valor-ldr').innerText = texto;
  }).catch(err => console.error(err));
}, 1000);
</script>
</head><body>
<div class='card'>
  <h1>Luminosidade (ADC)</h1>
  <div class='leitura' id='valor-ldr'>...</div>
  <p>Atualizando automaticamente a cada 1 segundo.</p>
</div>
</body></html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  
  analogReadResolution(12);

  WiFi.mode(WIFI_AP);

  if (WiFi.softAP(ssid, password)) {
    Serial.println("Access Point successfully started!");
  } else {
    Serial.println("Failed to start Access Point.");
  }

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    int valorLdr = analogRead(ldrPin);
    request->send(200, "text/plain", String(valorLdr));
  });

  server.begin();
}

void loop() {
}