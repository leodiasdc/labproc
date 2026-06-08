#include <WiFi.h>
#include <WebServer.h>

const int LED_PIN = 2;          
const int LEDC_CHANNEL_LED = 0; 
int ledDutyResolution = 8;      
int ledFrequency = 1000;        

const char* ap_ssid = "ESP32_Controle";
const char* ap_password = "password123";

WebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Control Dashboard</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; background-color: #f4f4f9; color: #333; margin:0; padding:20px; }
        .card { background: white; max-width: 500px; margin: 20px auto; padding: 20px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }
        h1 { color: #0066cc; }
        .slider { width: 80%; margin: 15px 0; }
        .value-display { font-weight: bold; color: #555; }
    </style>
</head>
<body>
    <div class="card">
        <h1>ESP32 Control Dashboard</h1>
        <hr>
        <h3>Controle do LED</h3>
        <p>Brilho (Duty Cycle): <input type="range" id="brightSlider" min="0" max="255" value="128" class="slider" onchange="updateLED()"></p>
        <p>Display Brilho: <span id="brightVal" class="value-display">128</span></p>
        
        <p>Frequência (Hz): <input type="range" id="freqSlider" min="10" max="5000" step="10" value="1000" class="slider" onchange="updateLED()"></p>
        <p>Display Frequência: <span id="freqVal" class="value-display">1000 Hz</span></p>
    </div>

    <script>
        function updateLED() {
            var brightness = document.getElementById("brightSlider").value;
            var frequency = document.getElementById("freqSlider").value;
            document.getElementById("brightVal").innerText = brightness;
            document.getElementById("freqVal").innerText = frequency + " Hz";
            
            fetch(`/setled?duty=${brightness}&freq=${frequency}`);
        }
    </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  ledcSetup(LEDC_CHANNEL_LED, ledFrequency, ledDutyResolution);
  ledcAttachPin(LED_PIN, LEDC_CHANNEL_LED);

  Serial.println("\nConfigurando Ponto de Acesso (SoftAP)...");
  WiFi.softAP(ap_ssid, ap_password);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Endereço de IP do AP: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, [](){
    server.send(200, "text/html", index_html);
  });

  server.on("/setled", HTTP_GET, [](){
    if (server.hasArg("duty") && server.hasArg("freq")) {
      int duty = server.arg("duty").toInt();
      int freq = server.arg("freq").toInt();
      
      ledcWriteTone(LEDC_CHANNEL_LED, freq);
      ledcWrite(LEDC_CHANNEL_LED, duty);
    }
    server.send(200, "text/plain", "OK");
  });

  server.begin();
  Serial.println("Servidor HTTP iniciado.");
}

void loop() {
  server.handleClient();
  delay(2); 
}