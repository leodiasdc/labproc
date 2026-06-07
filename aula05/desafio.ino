#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const int LED_PIN = 2;
const int SERVO_PIN = 4;
const int LEDC_CHANNEL_LED = 0;
const int LEDC_CHANNEL_SERVO = 1;

int ledFreq = 1000;
const int ledRes = 8;
const int servoFreq = 50;
const int servoRes = 16;

const char* ssid = "Sua_Rede_WiFi";
const char* password = "Seu_Password";

AsyncWebServer server(80);

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
            <h3>Slider 1: Intensidade do LED <span id="brightVal" class="value-display">128</span></h3>
            <input type="range" id="brightSlider" min="0" max="255" value="128" class="slider" onchange="updateSystem()">
            
            <label style="font-weight: bold; font-size: 0.9em;">Frequência do LED: <span id="freqVal" style="color:#2980b9;">1000 Hz</span></label>
            <input type="range" id="freqSlider" min="10" max="5000" step="10" value="1000" class="slider" onchange="updateSystem()">
        </div>

        <div class="control-group">
            <h3>Slider 2: Posição do Servo <span id="servoVal" class="value-display">90&deg;</span></h3>
            <input type="range" id="servoSlider" min="0" max="180" value="90" class="slider" onchange="updateSystem()">
        </div>
    </div>

    <script>
        function updateSystem() {
            var duty = document.getElementById("brightSlider").value;
            var freq = document.getElementById("freqSlider").value;
            var angle = document.getElementById("servoSlider").value;

            document.getElementById("brightVal").innerText = duty;
            document.getElementById("freqVal").innerText = freq + " Hz";
            document.getElementById("servoVal").innerHTML = angle + "&deg;";

            fetch(`/update?duty=${duty}&freq=${freq}&angle=${angle}`);
        }
    </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);

  ledcSetup(LEDC_CHANNEL_LED, ledFreq, ledRes);
  ledcAttachPin(LED_PIN, LEDC_CHANNEL_LED);

  ledcSetup(LEDC_CHANNEL_SERVO, servoFreq, servoRes);
  ledcAttachPin(SERVO_PIN, LEDC_CHANNEL_SERVO);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_p(200, "text/html", index_html);
  });

  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("duty") && request->hasParam("freq")) {
      int duty = request->getParam("duty")->value().toInt();
      int freq = request->getParam("freq")->value().toInt();
      
      ledcWriteTone(LEDC_CHANNEL_LED, freq);
      ledcWrite(LEDC_CHANNEL_LED, duty);
    }
    if (request->hasParam("angle")) {
      int angle = request->getParam("angle")->value().toInt();
      int servoDuty = map(angle, 0, 180, 3276, 6553);
      ledcWrite(LEDC_CHANNEL_SERVO, servoDuty);
    }
    request->send(200, "text/plain", "OK");
  });

  server.begin();
}

void loop() {} // loop vazio - cpu livre