#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESP32Servo.h>

const int LED_PIN = 2;
const int SERVO_PIN = 4;

int ledFreq = 1000;
const int ledRes = 13; 

const char* ssid = "Sua_Rede_WiFi";
const char* password = "Seu_Password";

AsyncWebServer server(80);

Servo meuServo;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Control Dashboard Integrado</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; background-color: #eef2f7; color: #333; margin:0; padding:20px; }
        .card { background: white; max-width: 550px; margin: 20px auto; padding: 25px; border-radius: 12px; box-shadow: 0 4px 15px rgba(0,0,0,0.15); }
        h1 { color: #2c3e50; border-bottom: 2px solid #34495e; padding-bottom: 10px; }
        .control-group { background: #f8f9fa; border: 1px solid #e9ecef; border-radius: 8px; padding: 15px; margin: 15px 0; text-align: left; }
        .control-group h3 { margin-top: 0; color: #2980b9; }
        .slider { width: 100%; height: 8px; border-radius: 5px; outline: none; margin: 10px 0; }
        .value-display { font-weight: bold; color: #e74c3c; float: right; }
    </style>
</head>
<body>
    <div class="card">
        <h1>ESP32 Control Dashboard</h1>
        
        <div class="control-group">
            <h3>Intensidade do LED <span id="brightVal" class="value-display">128</span></h3>
            <input type="range" id="brightSlider" min="0" max="255" value="128" class="slider" oninput="updateSystem()">
            
            <label style="font-weight: bold; font-size: 0.9em;">Frequência do LED: <span id="freqVal" style="color:#2980b9;">1000 Hz</span></label>
            <input type="range" id="freqSlider" min="10" max="5000" step="10" value="1000" class="slider" oninput="updateSystem()">
        </div>

        <div class="control-group">
            <h3>Posição do Servo <span id="servoVal" class="value-display">90&deg;</span></h3>
            <input type="range" id="servoSlider" min="0" max="180" value="90" class="slider" oninput="updateSystem()">
        </div>
    </div>

    <script>
        function updateSystem() {
            var duty = document.getElementById("brightSlider").value;
            var freq = document.getElementById("freqSlider").value;
            var angle = document.getElementById("servoSlider").value;

            // Atualização imediata do texto na tela (UX fluida)
            document.getElementById("brightVal").innerText = duty;
            document.getElementById("freqVal").innerText = freq + " Hz";
            document.getElementById("servoVal").innerHTML = angle + "&deg;";

            // Requisição Assíncrona Não-Bloqueante (RNF3)
            fetch(`/update?duty=${duty}&freq=${freq}&angle=${angle}`);
        }
    </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  ledcAttach(LED_PIN, ledFreq, ledRes);

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  
  meuServo.setPeriodHertz(50); 
  meuServo.attach(SERVO_PIN, 1000, 2000); 
  meuServo.write(90); 

  WiFi.mode(WIFI_AP);

  if (WiFi.softAP(ssid, password)) {
    Serial.println("Access Point successfully started!");
  } else {
    Serial.println("Failed to start Access Point.");
  }

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
    Serial.print("."); 
  }
  Serial.print("\nConectado com sucesso! IP Local: "); 
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", index_html);
  });

  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request){
    
    if (request->hasParam("duty") && request->hasParam("freq")) {
      int duty_8bit = request->getParam("duty")->value().toInt();
      int freq = request->getParam("freq")->value().toInt();

      int duty_13bit = map(duty_8bit, 0, 255, 0, 8191);
      
      ledcChangeFrequency(LED_PIN, freq, ledRes); 
      ledcWrite(LED_PIN, duty_13bit);
    }
    
    if (request->hasParam("angle")) {
      int angle = request->getParam("angle")->value().toInt();
      meuServo.write(angle);
    }
    
    request->send(200, "text/plain", "OK");
  });

  server.begin();
}

void loop() {} // loop vazio - cpu livre