const int pinBotao = 3;
const int pinLed = 2;

int estadoLed = LOW;
int btAgora = LOW;
int btAntes = LOW;
unsigned long tempo = 0;

void setup() {
  pinMode(pinBotao, INPUT);
  pinMode(pinLed, OUTPUT);
  digitalWrite(pinLed, estadoLed);
}

void loop() {
  int leitura = digitalRead(pinBotao);

  if (leitura != btAntes) {
    tempo = millis();
  }

  if ((millis() - tempo) > 50) {
    if (leitura != btAgora) {
      btAgora = leitura;

      if (btAgora == HIGH) {
        estadoLed = !estadoLed;
        digitalWrite(pinLed, estadoLed);
      }
    }
  }

  btAntes = leitura;
}