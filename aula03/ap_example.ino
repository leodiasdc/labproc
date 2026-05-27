/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-async-web-server-espasyncwebserver-library/
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "iPhone";
const char* password = "07092005";

const long int PIN_LED1 = 23;
const long int PIN_LED2 = 22;
const long int PIN_LED3 = 21;
const long int PIN_LED4 = 19;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

int8_t parse_to_int8(String a) ->  {
  long int value = strtol(a.c_str(), NULL, 2);
  int8_t result = (int8_t)value;
}

void setGPIOs(int8_t value) {
  digitalWrite(PIN_LED1, value & 0x01);
  digitalWrite(PIN_LED2, (value >> 1) & 0x01);
  digitalWrite(PIN_LED3, (value >> 2) & 0x01);
  digitalWrite(PIN_LED4, (value >> 3) & 0x01);
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Seta os pinos dos LEDs para output
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_LED3, OUTPUT);
  pinMode(PIN_LED4, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/calc?a=<valueA>&b=<valueB>&op=<operation>
  server.on("/calc", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String a, b, op;
    // GET input1 value on <ESP_IP>/calc?a=<valueA>&b=<valueB>&op=<operation>
    if (request->hasParam("a") && request->hasParam("b") && request->hasParam("op")) {
      a = request->getParam("a")->value();
      b = request->getParam("b")->value();
      op = request->getParam("op")->value();
    }
    int8_t valueA = parse_to_int8(a);
    int8_t valueB = parse_to_int8(b);

    if (op == "sum") {
      int8_t result = (valueA + valueB) & 0x0F; 
      request->send(200, "text/plain", String(result));
    } else if (op == "sub") {
      int8_t result = (valueA - valueB) & 0x0F; 
      request->send(200, "text/plain", String(result));
    } else {
      request->send(400, "text/plain", "Invalid operation");
    }
    
    setGPIOs(result);
    request->send(200, "text/plain", "OK");
  });

  // Start server
  server.begin();
}

void loop() {

}


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP32 Calculator</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    body {max-width: 600px; margin:0px auto; padding: 25px;}
    input {width: 100px; font-size: 1.2rem; margin: 8px;}
    button {font-size: 1.1rem; padding: 10px 14px; margin: 8px;}
    #result {font-size: 1.4rem; margin-top: 20px; display: block;}
  </style>
</head>
<body>
  <h2>ESP32 Calculator</h2>
  <input type="number" id="valueA" placeholder="Value A">
  <input type="number" id="valueB" placeholder="Value B">
  <div>
    <button onclick="sendCalc('sum')">Sum</button>
    <button onclick="sendCalc('sub')">Subtract</button>
  </div>
  <span id="result">Result: </span>
<script>
function sendCalc(op) {
  var a = document.getElementById('valueA').value;
  var b = document.getElementById('valueB').value;
  var xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (xhr.readyState === XMLHttpRequest.DONE) {
      if (xhr.status === 200) {
        document.getElementById('result').innerText = 'Result: ' + xhr.responseText;
      } else {
        document.getElementById('result').innerText = 'Error';
      }
    }
  };
  xhr.open('GET', '/calc?a=' + encodeURIComponent(a) + '&b=' + encodeURIComponent(b) + '&op=' + encodeURIComponent(op), true);
  xhr.send();
}
</script>
</body>
</html>
)rawliteral";