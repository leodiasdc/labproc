#include <stdio.h>
#include <lgpio.h>
#include <unistd.h>

// Definição dos pinos BCM (ajuste conforme sua montagem)
int linhas[] = {17, 27, 22, 5}; // GPIOs conectados às linhas
int colunas[] = {6, 13, 19, 26}; // GPIOs conectados às colunas

char teclas[4][4] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

int main() {
    int h = lgGpiochipOpen(0);
    if (h < 0) { printf("Erro ao abrir GPIO\n"); return 1; }

    // Configura linhas como saída e colunas como entrada (com pull-up)
    for(int i=0; i<4; i++) {
        lgGpioClaimOutput(h, 0, linhas[i], 1); // Inicia em HIGH
        lgGpioClaimInput(h, 0, colunas[i]);
    }

    printf("Aguardando teclas...\n");

    while(1) {
        for(int l=0; l<4; l++) {
            lgGpioWrite(h, linhas[l], 0); // Ativa a linha atual (GND)

            for(int c=0; c<4; c++) {
                if(lgGpioRead(h, colunas[c]) == 0) {
                    printf("Tecla pressionada: %c\n", teclas[l][c]);
                    usleep(200000); // Debounce simples
                }
            }
            lgGpioWrite(h, linhas[l], 1); // Desativa a linha
        }
        usleep(10000); // Pequena pausa no loop
    }

    lgGpiochipClose(h);
    return 0;
}