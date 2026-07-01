#include <stdio.h>
#include <stdlib.h>

#define MASK_4BIT 0x0F

int main() {
    int num1, num2;
    char operador;
    int resultado;
    
    printf("Digite o primeiro inteiro (4 bits): ");
    scanf("%d", &num1);
    
    printf("Digite o segundo inteiro (4 bits): ");
    scanf("%d", &num2);
    
    printf("Digite o operador (+, -, *, !): ");
    scanf(" %c", &operador);
    
    num1 = num1 & MASK_4BIT;
    num2 = num2 & MASK_4BIT;
    
    switch(operador) {
        case '+':
            resultado = (num1 + num2) & MASK_4BIT;
            printf("Resultado: %d\n", resultado);
            break;
        case '-':
            resultado = (num1 - num2) & MASK_4BIT;
            printf("Resultado: %d\n", resultado);
            break;
        case '*':
            resultado = (num1 * num2) & MASK_4BIT;
            printf("Resultado: %d\n", resultado);
            break;
        case '!': {
            unsigned long long fatorial = 1;
            for(int i = 1; i <= num1; i++) {
                fatorial *= i;
            }
            resultado = fatorial & MASK_4BIT;
            printf("Resultado: %d\n", resultado);
            break;
        }
        default:
            printf("Operador inválido!\n");
    }
    
    return 0;
}