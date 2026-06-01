#include <WiFi.h>
#include <WebServer.h>

#define NUM_BITS 4  // alterar para 8, 16, 32 bits que todo o comportamento se adapta

typedef int32_t calc_t;
const calc_t MAX_LIMIT = (calc_t)((1ULL << (NUM_BITS - 1)) - 1);
const calc_t MIN_LIMIT = (calc_t)(-(1ULL << (NUM_BITS - 1)));

// credenciais do access point Wi-Fi
const char* ssid = "LABPROC";
const char* password = "12345678";

WebServer server(80);

// Definição dos pinos dos LEDs
const int PIN_LED1 = 7; // LSB
const int PIN_LED2 = 6; 
const int PIN_LED3 = 5; 
const int PIN_LED4 = 4; 

int64_t computeFactorial(calc_t n) {
  if (n <= 1) return 1;
  int64_t result = 1;
  for (int i = 2; i <= n; i++) {
    result *= i;
  }
  return result;
}

// forçar o inteiro a ter N bits
calc_t applyWordSizeLimits(int64_t rawValue) {
  // 1. Aplica a máscara para isolar apenas os N bits úteis
  uint64_t mask = (1ULL << NUM_BITS) - 1;
  uint64_t maskedValue = (uint64_t)rawValue & mask;

  // 2. Verifica se o bit de sinal de N bits está ativo
  uint64_t signBit = 1ULL << (NUM_BITS - 1);
  if (maskedValue & signBit) {
    // Se o bit de sinal estiver ativo, estende o sinal para o tipo nativo da memória (calc_t)
    return (calc_t)(maskedValue | ~(mask));
  } else {
    return (calc_t)maskedValue;
  }
}

// acionar LEDs
void setGPIOs(calc_t value) {
  uint8_t lsb_4bits = (uint8_t)(value & 0x0F);
  
  digitalWrite(PIN_LED1, (lsb_4bits >> 0) & 0x01);
  digitalWrite(PIN_LED2, (lsb_4bits >> 1) & 0x01);
  digitalWrite(PIN_LED3, (lsb_4bits >> 2) & 0x01);
  digitalWrite(PIN_LED4, (lsb_4bits >> 3) & 0x01);
}

// executa a operação aritmética e verifica overflow
calc_t executeOperation(calc_t a, calc_t b, String op, bool &overflow) {
  int64_t tempResult = 0;
  overflow = false;

  a = applyWordSizeLimits(a);
  b = applyWordSizeLimits(b);

  if (op == "add") {
    tempResult = (int64_t)a + b;
    // sinais iguais gerando "sinal oposto" (passando do range máximo aq)
    if ((a > 0 && b > 0 && tempResult > MAX_LIMIT) || 
        (a < 0 && b < 0 && tempResult < MIN_LIMIT)) {
      overflow = true;
    }
  } 
  else if (op == "sub") {
    tempResult = (int64_t)a - b;
    // sinais opostos passando do range máximo
    if ((a > 0 && b < 0 && tempResult > MAX_LIMIT) || 
        (a < 0 && b > 0 && tempResult < MIN_LIMIT)) {
      overflow = true;
    }
  } 
  else if (op == "mul") {
    tempResult = (int64_t)a * b;
    if (tempResult > MAX_LIMIT || tempResult < MIN_LIMIT) {
      overflow = true;
    }
  } 
  else if (op == "fact") {
    tempResult = computeFactorial(a);
    if (tempResult < 0 || tempResult > MAX_LIMIT) {
      overflow = true;
    }
  }

  return applyWordSizeLimits(tempResult);
}

void handleRoot() {
  const char html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP32 Advanced Calculator</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta charset="UTF-8">
  <style>
    html {font-family: Arial, sans-serif; display: inline-block; text-align: center; background-color: #f4f4f9;}
    body {max-width: 600px; margin: 40px auto; padding: 25px; background: white; border-radius: 8px; box-shadow: 0 4px 8px rgba(0,0,0,0.1);}
    input, select {font-size: 1.2rem; padding: 8px; margin: 10px; width: 80%; max-width: 200px; text-align: center;}
    button {font-size: 1.2rem; padding: 10px 20px; background-color: #007BFF; color: white; border: none; border-radius: 4px; cursor: pointer; margin-top: 15px;}
    button:hover {background-color: #0056b3;}
    .alert {color: #d9534f; font-weight: bold; margin-top: 15px; padding: 10px; background-color: #f2dede; border-radius: 4px; border: 1px solid #ebccd1; display: none;}
    .success {color: #3c763d; font-weight: bold; margin-top: 15px;}
    #box-resultado {margin-top: 25px; padding: 15px; border-top: 2px solid #eee;}
  </style>
</head>
<body>
  <h2>ESP32 Calculator — Extended Edition</h2>
  
  <div>
    <input type="number" id="valueA" placeholder="Operando A (Dec)">
    <br>
    <select id="operation" onchange="toggleInputB()">
      <option value="add">Soma (+)</option>
      <option value="sub">Subtração (-)</option>
      <option value="mul">Multiplicação (*)</option>
      <option value="fact">Fatorial (!)</option>
    </select>
    <br>
    <input type="number" id="valueB" placeholder="Operando B (Dec)">
  </div>
  
  <button onclick="calculate()">Calcular</button>

  <div id="box-resultado">
    <div id="overflowContainer" class="alert">AVISO: Ocorreu um estouro de capacidade (Overflow)!</div>
    <p class="success">Resultado Decimal: <span id="resDec">-</span></p>
    <p>Visualização Binária Equivalente: <span id="resBin">-</span></p>
    <p style="font-size: 0.85rem; color: #666;">Tempo de processamento no ESP32: <span id="benchTime">0</span> µs</p>
  </div>

  <script>
    function toggleInputB() {
      const op = document.getElementById("operation").value;
      const inputB = document.getElementById("valueB");
      if (op === "fact") {
        inputB.style.display = "none";
      } else {
        inputB.style.display = "inline-block";
      }
    }

    function calculate() {
      const valA = document.getElementById("valueA").value || "0";
      const valB = document.getElementById("valueB").value || "0";
      const op = document.getElementById("operation").value;
      const ovfAlert = document.getElementById("overflowContainer");

      ovfAlert.style.display = "none";

      fetch(`/calc?a=${valA}&b=${valB}&op=${op}`)
        .then(response => response.json())
        .then(data => {
          document.getElementById("resDec").innerText = data.resultadoDecimal;
          document.getElementById("resBin").innerText = data.resultadoBinario;
          document.getElementById("benchTime").innerText = data.tempoExecucaoUs;
          
          if(data.overflow) {
            ovfAlert.style.display = "block";
            ovfAlert.innerText = "AVISO: Ocorreu Overflow aritmético no ESP32! Exibindo resultado truncado.";
          }
        })
        .catch(err => console.error("Erro na requisição:", err));
    }
  </script>
</body>
</html>
)rawliteral";
  server.send(200, "text/html", html);
}

void handleCalculator() {

    calc_t valA = (calc_t)server.arg("a").toInt();
    calc_t valB = (calc_t)server.arg("b").toInt();
    String op = server.arg("op");

    bool overflow = false;

    uint32_t startTime = micros(); 
    calc_t result = executeOperation(valA, valB, op, overflow);
    uint32_t endTime = micros();

    uint32_t elapsed = endTime - startTime;
    
    setGPIOs(result);

    String binaryString = "";
    for (int i = NUM_BITS - 1; i >= 0; i--) {
        binaryString += ((result >> i) & 0x01) ? "1" : "0";
    }

    // geração da resposta padronizada em JSON para comunicação com o front
    String jsonResponse = "{";
    jsonResponse += "\"resultadoDecimal\":" + String((int)result) + ",";
    jsonResponse += "\"resultadoBinario\":\"" + binaryString + "\",";
    jsonResponse += "\"overflow\":" + String(overflow ? "true" : "false") + ",";
    jsonResponse += "\"tempoExecucaoUs\":" + String(elapsed);
    jsonResponse += "}";

    // log detalhado no console serial
    Serial.println("====== [PROCESSO DE EXECUÇÃO] ======");
    Serial.printf("Inputs: A = %d | B = %d | Operação = %s\n", (int)valA, (int)valB, op.c_str());
    Serial.printf("Saída Calculada (Dec): %d\n", (int)result);
    Serial.printf("Saída Binária: %s\n", binaryString.c_str());
    Serial.printf("Status de Overflow: %s\n", overflow ? "DETECTADO!" : "OK");
    Serial.printf("Duração do Ciclo: %u microssegundos\n", elapsed);
    Serial.println("------------------------------------");

    server.send(200, "application/json", jsonResponse);
}

void runNonRegressionTests() {
  Serial.println("\n>>> [PIPELINE] Inicializando Testes de Não-Regressão...");
  int testesPassaram = 0;
  int testesTotais = 0;

  auto assertTest = [&](const char* name, calc_t a, calc_t b, String op, calc_t expectedRes, bool expectedOvf) {
    testesTotais++;
    bool currentOvf = false;
    calc_t currentRes = executeOperation(a, b, op, currentOvf);

    if (currentRes == expectedRes && currentOvf == expectedOvf) {
      Serial.printf("  [PASS] %s -> Entrada: (%d, %d) | Res Mascarado Esperado: %d | Ovf: %s\n", 
                    name, (int)a, (int)b, (int)expectedRes, expectedOvf ? "Sim" : "Não");
      testesPassaram++;
    } else {
      Serial.printf("  [FAIL] %s -> Falhou! Obtido: %d (Ovf: %s) | Esperado: %d (Ovf: %s)\n", 
                    name, (int)currentRes, currentOvf ? "Sim" : "Não", (int)expectedRes, expectedOvf ? "Sim" : "Não");
    }
  };

  #if NUM_BITS == 4
    assertTest("Soma Padrão", 2, 2, "add", 4, false);
    assertTest("Soma com Overflow Positivo (5+4)", 5, 4, "add", -7, true); // 5 + 4 = 9 -> vira -7 em 4 bits
    assertTest("Subtração Padrão", 5, 2, "sub", 3, false);
    assertTest("Subtração com Overflow Negativo (-6-3)", -6, 3, "sub", 7, true); // -6 - 3 = -9 -> vira 7 em 4 bits
    assertTest("Multiplicação Sem Estouro", 3, 2, "mul", 6, false);
    assertTest("Multiplicação Com Estouro (4*2)", 4, 2, "mul", -8, true);  // 4 * 2 = 8 -> vira -8 em 4 bits
    assertTest("Fatorial Válido", 3, 0, "fact", 6, false);
    assertTest("Fatorial com Overflow (4!)", 4, 0, "fact", -8, true);    // 4! = 24 -> 24 % 16 = 8 -> vira -8 em 4 bits
  #else
  #endif

  Serial.printf(">>> [TESTS COMPLETE] %d/%d sub-rotinas passaram com sucesso.\n\n", testesPassaram, testesTotais);
}

// ==========================================
// INICIALIZAÇÃO DO SISTEMA
// ==========================================
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=========================================");
  Serial.println("Inicializando Firmware ESP32 Refatorado...");
  Serial.printf("Configuração Atual da Palavra: %d bits\n", NUM_BITS);
  Serial.println("=========================================");

  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_LED3, OUTPUT);
  pinMode(PIN_LED4, OUTPUT);
  setGPIOs(0); 

  // execução do pipeline de testes antes do servidor subir
  runNonRegressionTests();

  // Inicialização do Wi-Fi Access Point Autônomo
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  
  Serial.println("=========================================");
  Serial.print("Rede Wi-Fi Hospedada: "); Serial.println(ssid);
  Serial.print("Endereço IP do Web Server: "); Serial.println(IP);
  Serial.println("=========================================");

  // Definição das rotas HTTP do servidor
  server.on("/", handleRoot);
  server.on("/calc", handleCalculator);

  server.begin();
  Serial.println("Servidor HTTP ativo e escutando na porta 80.");
}

void loop() {
  server.handleClient();
}