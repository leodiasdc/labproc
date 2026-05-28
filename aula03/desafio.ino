#include <Arduino.h>

const int LED_PINOS[] = {7, 6, 5, 4};

void atualizarLeds(uint8_t valor) {
  for (int i = 0; i < 4; i++) {
    int bit = (valor >> i) & 1;
    digitalWrite(LED_PINOS[i], bit);
  }
}

void imprimirBinario(uint8_t valor) {
  for (int i = 3; i >= 0; i--) {
    int bit = (valor >> i) & 1;
    Serial.print(bit);
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); } 

  for (int i = 0; i < 4; i++) {
    pinMode(LED_PINOS[i], OUTPUT);
  }
  atualizarLeds(0);

  Serial.println("\n=== CALCULADORA: COMPLEMENTO DE 1 (4 BITS) ===");
  Serial.println("Digite a conta no formato: A + B ou A - B (entre -7 e +7)");
  Serial.println("==============================================");
}

void loop() {
  if (Serial.available() > 0) {
    int numA = Serial.parseInt();
    char op = Serial.read();
    
    while (op == ' ') { 
      op = Serial.read(); 
    }
    
    int numB = Serial.parseInt();
    
    while (Serial.available() > 0) { 
      Serial.read(); 
    }

    if (numA < -7 || numA > 7 || numB < -7 || numB > 7) {
      Serial.println("Erro: Os números devem estar entre -7 e +7.");
      return;
    }

    uint8_t compA;
    if (numA >= 0) {
      compA = numA;
    } else {
      compA = ~(-numA) & 0x0F;
    }

    uint8_t compB;
    if (numB >= 0) {
      compB = numB;
    } else {
      compB = ~(-numB) & 0x0F;
    }

    Serial.println("\n=========================================");
    Serial.printf("Operação: %d %c %d\n", numA, op, numB);
    Serial.print("A em Binário: "); imprimirBinario(compA);
    Serial.print("B em Binário: "); imprimirBinario(compB);
    Serial.println("-----------------------------------------");

    uint8_t operandoB = compB;
    if (op == '-') {
      operandoB = ~compB & 0x0F;
      Serial.print("B Negado (NOT B): "); imprimirBinario(operandoB);
    } else if (op != '+') {
      Serial.println("Operador inválido! Use apenas '+' ou '-'.");
      return;
    }

    uint16_t somaBruta = compA + operandoB;
    if (somaBruta > 15) {
      somaBruta = (somaBruta & 0x0F) + 1;
    }
    uint8_t resultadoBinario = somaBruta & 0x0F;

    uint8_t sinalA = (compA >> 3) & 1;
    uint8_t sinalB = (operandoB >> 3) & 1;
    uint8_t sinalR = (resultadoBinario >> 3) & 1;
    
    bool overflow = false;
    if (sinalA == sinalB && sinalA != sinalR) {
      if (!((compA == 0x00 && operandoB == 0x0F) || (compA == 0x0F && operandoB == 0x00))) {
        overflow = true;
      }
    }

    if (overflow) {
      atualizarLeds(0);
      Serial.println("STATUS: OVERFLOW! O resultado estourou os limites.");
    } else {
      atualizarLeds(resultadoBinario);

      int resultadoDecimal;
      int bitSinal = (resultadoBinario >> 3) & 1;
      if (bitSinal == 1) {
        resultadoDecimal = -((~resultadoBinario) & 0x0F);
      } else {
        resultadoDecimal = resultadoBinario;
      }

      Serial.print("RESULTADO BINÁRIO: "); imprimirBinario(resultadoBinario);
      Serial.print("RESULTADO DECIMAL: "); Serial.print(resultadoDecimal);

      if (resultadoBinario == 0x00) {
        Serial.println(" (+0)");
      } else if (resultadoBinario == 0x0F) {
        Serial.println(" (-0)");
      } else {
        Serial.println();
      }
    }
    Serial.println("=========================================");
  }
}