import os
import sys
import json
import queue
import sounddevice as sd
from vosk import Model, KaldiRecognizer

# Fila para gerenciar o áudio recebido do microfone
q = queue.Queue()

def callback(indata, frames, time, status):
    if status:
        print(status, file=sys.stderr)
    q.put(bytes(indata))

# O Vosk vai baixar o modelo automaticamente se ele não existir no cache do sistema
print("⏳ Carregando o modelo de voz em português... Aguarde.")
try:
    model = Model(model_name="vosk-model-small-pt-0.3")
    rec = KaldiRecognizer(model, 16000)
except Exception as e:
    print(f"❌ Erro ao carregar ou baixar o modelo: {e}")
    sys.exit(1)

print("\n🤖 [Alexa Local]: Inicializada com sucesso!")
print("🎙️ Modo de Espera. Diga 'ativar' para falar um comando.")
print("-" * 60)

# Controle se estamos esperando a palavra-chave ou o comando real
modo_comando = False

try:
    # Captura o microfone padrão a 16kHz
    with sd.RawInputStream(samplerate=16000, blocksize=8000, dtype='int16',
                           channels=1, callback=callback):
        while True:
            data = q.get()
            if rec.AcceptWaveform(data):
                resultado = json.loads(rec.Result())
                texto = resultado.get("text", "").lower().strip()
                
                if not texto:
                    continue

                if not modo_comando:
                    # ESTADO 1: Procurando a palavra 'ativar'
                    if "ativar" in texto:
                        print("\n🔔 [Alexa]: Diga o comando...")
                        modo_comando = True
                    else:
                        print(f"💤 Ouvido (Ignorado): '{texto}'")
                else:
                    # ESTADO 2: Processando o comando após a ativação
                    print(f"🧠 [Comando Recebido]: '{texto}'")
                    
                    # Suas regras de automação/comandos:
                    if "ligar a luz" in texto or "acender" in texto:
                        print("💡 Executando: Ligando as luzes do quarto...")
                    elif "horas" in texto:
                        print("⏰ Executando: Buscando o horário atual do sistema...")
                    elif "desligar" in texto or "cancelar" in texto:
                        print("😴 Executando: Cancelado.")
                    else:
                        print("❓ Comando não reconhecido.")
                    
                    # Retorna ao modo de espera
                    print("-" * 60)
                    print("🎙️ Modo de Espera. Diga 'ativar'.")
                    modo_comando = False

except KeyboardInterrupt:
    print("\n\n👋 Assistente encerrado pelo usuário.")
except Exception as e:
    print(f"\n⚠️ Erro de Áudio/Sistema: {e}")
    print("Dica: Se der erro de 'Default input device', o WSL2 não está conseguindo acessar seu microfone.")
