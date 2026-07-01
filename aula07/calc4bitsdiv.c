#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MASK_4BIT 0x0F

int32_t estender_sinal_4bits(int32_t val) {
    val = val & MASK_4BIT;
    if (val & 0x08) {
        return (int32_t)(val | ~MASK_4BIT);
    }
    return val;
}

int main() {
    int num1, num2;
    char operador;
    int resultado_puro;
    int resultado_sinalizado;
    int overflow = 0;
    
    printf("Digite o primeiro inteiro (4 bits): ");
    if (scanf("%d", &num1) != 1) return 1;
    
    printf("Digite o operador (+, -, *, /, !): ");
    if (scanf(" %c", &operador) != 1) return 1;
    
    if (operador != '!') {
        printf("Digite o segundo inteiro (4 bits): ");
        if (scanf("%d", &num2) != 1) return 1;
    }
    
    int n1 = estender_sinal_4bits(num1);
    int n2 = estender_sinal_4bits(num2);
    
    switch(operador) {
        case '+':
            resultado_sinalizado = n1 + n2;
            if (resultado_sinalizado < -8 || resultado_sinalizado > 7) {
                overflow = 1;
            }
            break;
        case '-':
            resultado_sinalizado = n1 - n2;
            if (resultado_sinalizado < -8 || resultado_sinalizado > 7) {
                overflow = 1;
            }
            break;
        case '*':
            resultado_sinalizado = n1 * n2;
            if (resultado_sinalizado < -8 || resultado_sinalizado > 7) {
                overflow = 1;
            }
            break;
        case '/':
            if (n2 == 0) {
                printf("Divisão por zero não permitida!\n");
                return 1;
            }
            if (n1 == -8 && n2 == -1) {
                overflow = 1;
                resultado_sinalizado = 8;
            } else {
                resultado_sinalizado = n1 / n2;
            }
            break;
        case '!':
            if (n1 < 0) {
                printf("Erro: Fatorial de número negativo não existe!\n");
                return 1;
            }
            if (n1 > 3) {
                overflow = 1;
            }
            unsigned long long fatorial = 1;
            for(int i = 1; i <= n1; i++) {
                fatorial *= i;
                if (fatorial > 7 && n1 <= 3) {
                    overflow = 1;
                }
            }
            resultado_sinalizado = (int)fatorial;
            break;
        default:
            printf("Operador inválido!\n");
            return 1;
    }
    
    resultado_puro = resultado_sinalizado & MASK_4BIT;
    int exibicao_final = estender_sinal_4bits(resultado_puro);
    
    printf("Resultado: %d\n", exibicao_final);
    
    if (overflow) {
        printf("AVISO: Ocorreu OVERFLOW!\n");
    }
    
    return 0;
}