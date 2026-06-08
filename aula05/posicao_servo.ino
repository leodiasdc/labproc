#include <WiFi.h>
#include <WebServer.h>

const int SERVO_PIN = 4;          
const int LEDC_CHANNEL_SERVO = 1; 
const int SERVO_FREQ = 50;         
const int SERVO_RES = 16;         

const char* ap_ssid = "ESP32_Servo";
const char* ap_password = "password123";

WebServer server(80);

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
        <p>Posição: 0&deg; <input type="range" id="servoSlider" min="0" max="180" value="90" class="slider" onchange="updateServo()"> 180&deg;</p>
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

  Serial.println("\nConfigurando Ponto de Acesso (SoftAP)...");
  WiFi.softAP(ap_ssid, ap_password);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Endereço de IP do AP: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, [](){
    server.send(200, "text/html", index_html);
  });

  server.on("/setservo", HTTP_GET, [](){
    if (server.hasArg("angle")) {
      int angle = server.arg("angle").toInt();
      
      int duty = map(angle, 0, 180, 3276, 6553);
      ledcWrite(LEDC_CHANNEL_SERVO, duty);
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