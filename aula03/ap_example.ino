#include <WiFi.h>
#include <WebServer.h>

// Defina o nome da rede Wi-Fi e a senha que o ESP32 vai criar
const char* ssid = "LABPROC";
const char* password = "12345678";

// Inicializa o servidor web na porta padrão HTTP (porta 80)
WebServer server(80);

const long int PIN_LED1 = 7;
const long int PIN_LED2 = 6;
const long int PIN_LED3 = 5;
const long int PIN_LED4 = 4;

// CORREÇÃO: Adicionado o 'return' e alterado a base de 2 para 10 (Decimal)
int8_t parse_to_int8(String a) {
  long int value = strtol(a.c_str(), NULL, 10); 
  return (int8_t)value; 
}

void setGPIOs(int8_t value) {
  digitalWrite(PIN_LED1, value & 0x01);
  digitalWrite(PIN_LED2, (value >> 1) & 0x01);
  digitalWrite(PIN_LED3, (value >> 2) & 0x01);
  digitalWrite(PIN_LED4, (value >> 3) & 0x01);
}

// Função que monta a página HTML que será exibida no navegador
void handleRoot() {
  const char html[] PROGMEM = R"rawliteral(
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
    #result {font-size: 1.4rem; margin-top: 20px; display: block; font-weight: bold;}
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
async function sendCalc(op) {
  var a = document.getElementById('valueA').value;
  var b = document.getElementById('valueB').value;

  if (a === "" || b === "") {
    document.getElementById("result").innerHTML = "Result: Por favor, preencha ambos os campos.";
    return;
  }

  try {
    let resposta = await fetch(`/calc?a=${a}&b=${b}&op=${op}`);
    
    if (resposta.ok) {
      let texto = await resposta.text();
      document.getElementById("result").innerHTML = "Result: " + texto;
    } else {
      document.getElementById("result").innerHTML = "Result: Erro no servidor.";
    }
  } catch (erro) {
    document.getElementById("result").innerHTML = "Result: Erro de conexão.";
    console.error("Erro na requisição:", erro);
  }
}
</script>
</body>
</html>
)rawliteral";
  
  server.send(200, "text/html", html);
}

void calcular(){
    String a = server.arg("a");
    String b = server.arg("b");
    String op = server.arg("op");

    int8_t valueA = parse_to_int8(a);
    int8_t valueB = parse_to_int8(b);
    int8_t result = 0;

    if (op == "sum") {
      result = (valueA + valueB) & 0x0F;
    } else {
      result = (valueA - valueB) & 0x0F; 
    } 

    setGPIOs(result);

    // Prints de Debug no Monitor Serial
    Serial.print("Value A: "); Serial.println(valueA);
    Serial.print("Operation: "); Serial.println(op);
    Serial.print("Value B: "); Serial.println(valueB);
    Serial.print("Resultado: "); Serial.println(result);
    Serial.println("-----------------------");

    // Retorna o resultado como Texto Plano (mais adequado para o fetch receber)
    server.send(200, "text/plain", String(result));
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nIniciando configuração do ESP32...");

  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_LED3, OUTPUT);
  pinMode(PIN_LED4, OUTPUT);

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  
  Serial.println("=========================================");
  Serial.print("Rede Wi-Fi criada: "); Serial.println(ssid);
  Serial.print("Endereço IP do Servidor: "); Serial.println(IP);
  Serial.println("=========================================");

  server.on("/", handleRoot);
  server.on("/calc", calcular);
  server.begin();
  Serial.println("Servidor HTTP iniciado com sucesso.");
}

void loop() {
  server.handleClient();
  delay(2);
}