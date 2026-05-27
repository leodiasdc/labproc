#include <Arduino.h>

const long int PIN_LED1 = 7; 
const long int PIN_LED2 = 6; 
const long int PIN_LED3 = 5;
const long int PIN_LED4 = 4; 

void setGPIOs(uint8_t value) {
  digitalWrite(PIN_LED1, value & 0x01);
  digitalWrite(PIN_LED2, (value >> 1) & 0x01);
  digitalWrite(PIN_LED3, (value >> 2) & 0x01);
  digitalWrite(PIN_LED4, (value >> 3) & 0x01);
}

uint8_t paraComplementoDeUm(int decimal) {
  if (decimal >= 0) {
    return (uint8_t)(decimal & 0x0F);
  } else {
    uint8_t positivo = (uint8_t)(-decimal) & 0x0F;
    return (uint8_t)(~positivo & 0x0F);
  }
}

int paraDecimal(uint8_t comp1) {
  if (comp1 & 0x08) {
    return -((~comp1) & 0x0F);
  }
  return comp1 & 0x0F;
}

String stringBinaria(uint8_t valor) {
  String str = "";
  for (int i = 3; i >= 0; i--) {
    str += ((valor >> i) & 0x01) ? "1" : "0";
  }
  return str;
}

uint8_t somarComplementoDeUm(uint8_t a, uint8_t b, bool &overflow) {
  uint16_t somaBruta = (uint16_t)a + (uint16_t)b;

  if (somaBruta & 0x10) {
    somaBruta = (somaBruta & 0x0F) + 1; 
  }
  
  uint8_t resultado = somaBruta & 0x0F;

  uint8_t sinalA = a & 0x08;
  uint8_t sinalB = b & 0x08;
  uint8_t sinalR = resultado & 0x08;
  
  if (sinalA == sinalB && sinalA != sinalR) {
    if (!((a == 0x00 && b == 0x0F) || (a == 0x0F && b == 0x00))) {
      overflow = true;
    }
  }
  
  return resultado;
}

void processarCalculo(int numA, char op, int numB) {
  if (numA < -7 || numA > 7 || numB < -7 || numB > 7) {
    Serial.println("Erro: Entradas fora do limite para 4 bits em Comp. de 1 (-7 a +7).");
    return;
  }

  uint8_t compA = paraComplementoDeUm(numA);
  uint8_t compB = paraComplementoDeUm(numB);
  uint8_t resultadoBinario = 0;
  bool overflow = false;

  Serial.println("\n=========================================");
  Serial.print("Operação: "); Serial.print(numA); Serial.print(" "); Serial.print(op); Serial.print(" "); Serial.println(numB);
  Serial.print("A em Binário: "); Serial.println(stringBinaria(compA));
  Serial.print("B em Binário: "); Serial.println(stringBinaria(compB));
  Serial.println("-----------------------------------------");

  if (op == '+') {
    resultadoBinario = somarComplementoDeUm(compA, compB, overflow);
  } 
  else if (op == '-') {
    uint8_t compBNegado = ~compB & 0x0F; 
    Serial.print("B Negado (NOT B): "); Serial.println(stringBinaria(compBNegado));
    resultadoBinario = somarComplementoDeUm(compA, compBNegado, overflow);
  } 
  else {
    Serial.println("Operador inválido! Use apenas '+' ou '-'.");
    return;
  }

  if (overflow) {
    setGPIOs(0); 
    Serial.println("STATUS: OVERFLOW! O resultado estourou os limites de 4 bits.");
  } else {
    setGPIOs(resultadoBinario); 
    
    int resultadoDecimal = paraDecimal(resultadoBinario);
    Serial.print("RESULTADO BINÁRIO: "); Serial.println(stringBinaria(resultadoBinario));
    Serial.print("RESULTADO DECIMAL: "); Serial.print(resultadoDecimal);
    
    if (resultadoBinario == 0x00) Serial.println(" (+0)");
    else if (resultadoBinario == 0x0F) Serial.println(" (-0)");
    else Serial.println();
  }
  Serial.println("=========================================");
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); } 
  
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_LED3, OUTPUT);
  pinMode(PIN_LED4, OUTPUT);
  
  setGPIOs(0); 

  Serial.println("\n=== CALCULADORA SERIAL: COMPLEMENTO DE UM (4 BITS) ===");
  Serial.println("Instruções: Digite a equação separada por espaços.");
  Serial.println("Exemplos válidos: '5 + 2' , '3 - 7' , '-4 + -2'");
  Serial.println("Limites aceitáveis: -7 a +7");
  Serial.println("======================================================");
}

void loop() {
  if (Serial.available() > 0) {
    int valorA = Serial.parseInt();
    char operador = Serial.read();
    
    while (operador == ' ') {
      operador = Serial.read();
    }
    
    int valorB = Serial.parseInt();
    
    while (Serial.available() > 0) {
      Serial.read();
    }

    processarCalculo(valorA, operador, valorB);
  }
}