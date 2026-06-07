#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const int SERVO_PIN = 4;          
const int LEDC_CHANNEL_SERVO = 1; 
const int SERVO_FREQ = 50;         
const int SERVO_RES = 16;         

const char* ssid = "Sua_Rede_WiFi";
const char* password = "Seu_Password";

AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Servo Dashboard</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; background-color: #f4f4f9; color: #333; margin:0; padding:20px; }
        .card { background: white; max-width: 500px; margin: 20px auto; padding: 20px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }
        h1 { color: #e67e22; }
        .slider { width: 80%; margin: 15px 0; }
        .value-display { font-weight: bold; color: #555; }
    </style>
</head>
<body>
    <div class="card">
        <h1>ESP32 Servo Dashboard</h1>
        <hr>
        <h3>Controle do Servomotor</h3>
        <p>Posição: $0^\circ$ <input type="range" id="servoSlider" min="0" max="180" value="90" class="slider" onchange="updateServo()"> $180^\circ$</p>
        <p>Ângulo Atual: <span id="servoVal" class="value-display">90&deg;</span></p>
    </div>

    <script>
        function updateServo() {
            var angle = document.getElementById("servoSlider").value;
            document.getElementById("servoVal").innerHTML = angle + "&deg;";
            fetch(`/setservo?angle=${angle}`);
        }
    </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);

  ledcSetup(LEDC_CHANNEL_SERVO, SERVO_FREQ, SERVO_RES);
  ledcAttachPin(SERVO_PIN, LEDC_CHANNEL_SERVO);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_p(200, "text/html", index_html);
  });

  server.on("/setservo", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("angle")) {
      int angle = request->getParam("angle")->value().toInt();
      
      int duty = map(angle, 0, 180, 3276, 6553);
      ledcWrite(LEDC_CHANNEL_SERVO, duty);
    }
    request->send(200, "text/plain", "OK");
  });

  server.begin();
}

void loop() {} // loop vazio - cpu livre