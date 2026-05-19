
const int PIN_VERDE   = 3;
const int PIN_AMARELO = 4;
const int PIN_VERMELHO = 5;

const unsigned long TEMPO_VERDE   = 3000; 
const unsigned long TEMPO_AMARELO = 1000;
const unsigned long TEMPO_VERMELHO = 4000;

void setup() {
  pinMode(PIN_VERDE, OUTPUT);
  pinMode(PIN_AMARELO, OUTPUT);
  pinMode(PIN_VERMELHO, OUTPUT);
  
}

void loop() {
  digitalWrite(PIN_VERDE, HIGH);
  delay(TEMPO_VERDE);
  digitalWrite(PIN_VERDE, LOW);
  
  digitalWrite(PIN_AMARELO, HIGH);
  delay(TEMPO_AMARELO);
  digitalWrite(PIN_AMARELO, LOW);

  digitalWrite(PIN_VERMELHO, HIGH);
  delay(TEMPO_VERMELHO);
  digitalWrite(PIN_VERMELHO, LOW);
}

