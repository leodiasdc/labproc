#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESP32Servo.h> 

const int SERVO_PIN = 4;

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
        <p>Posição: 0&deg; <input type="range" id="servoSlider" min="0" max="180" value="90" class="slider" oninput="updateServo()"> 180&deg;</p>
        <p>Ângulo Atual: <span id="servoVal" class="value-display">90&deg;</span></p>
    </div>

    <script>
        function updateServo() {
            var angle = document.getElementById("servoSlider").value;
            document.getElementById("servoVal").innerHTML = angle + "&deg;";
            fetch(/setservo?angle=${angle});
        }
    </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  
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

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", index_html);
  });

  server.on("/setservo", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("angle")) {
      int angle = request->getParam("angle")->value().toInt();
      
      meuServo.write(angle);
      
      Serial.print("Movendo para: ");
      Serial.print(angle);
      Serial.println(" graus.");
    }
    request->send(200, "text/plain", "OK");
  });

  server.begin();
}

void loop() {
  // Loop vazio
}