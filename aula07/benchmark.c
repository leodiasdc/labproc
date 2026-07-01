#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define ITERATIONS 10000000 

double calcular_ns(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
}

int main() {
    int64_t entrada1, entrada2 = 0;
    char operador;
    struct timespec start, end;
    double t4 = 0, t8 = 0, t16 = 0, t32 = 0;

    printf("Digite o primeiro número: ");
    if (scanf("%lld", &entrada1) != 1) return 1;

    printf("Digite o operador (+, -, *, !): ");
    if (scanf(" %c", &operador) != 1) return 1;

    if (operador != '!') {
        printf("Digite o segundo número: ");
        if (scanf("%lld", &entrada2) != 1) return 1;
    }

    // Inicialização das variáveis (simulando 4 bits com máscara 0x0F)
    uint8_t  a4  = ((uint8_t)entrada1) & 0x0F, b4  = ((uint8_t)entrada2) & 0x0F, r4 = 0;
    uint8_t  a8  = (uint8_t)entrada1,          b8  = (uint8_t)entrada2,          r8 = 0;
    uint16_t a16 = (uint16_t)entrada1,         b16 = (uint16_t)entrada2,         r16 = 0;
    uint32_t a32 = (uint32_t)entrada1,         b32 = (uint32_t)entrada2,         r32 = 0;

    printf("\n=== CALCULADORA BENCHMARK (10M de iterações) ===\n");
    
    if (operador == '!') {
        uint32_t val = (entrada1 < 0) ? 0 : (uint32_t)entrada1;
        
        // 4 BITS (Simulado via máscara)
        clock_gettime(CLOCK_MONOTONIC, &start);
        for(volatile int i = 0; i < ITERATIONS; i++) { 
            uint8_t fat4 = 1;
            for(uint8_t j = 1; j <= val; j++) { fat4 = (fat4 * j) & 0x0F; }
            r4 = fat4;
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        t4 = calcular_ns(start, end);

        // 8 BITS
        clock_gettime(CLOCK_MONOTONIC, &start);
        for(volatile int i = 0; i < ITERATIONS; i++) { 
            uint8_t fat8 = 1;
            for(uint8_t j = 1; j <= val; j++) { fat8 *= j; }
            r8 = fat8;
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        t8 = calcular_ns(start, end);

        // 16 BITS
        clock_gettime(CLOCK_MONOTONIC, &start);
        for(volatile int i = 0; i < ITERATIONS; i++) { 
            uint16_t fat16 = 1;
            for(uint16_t j = 1; j <= val; j++) { fat16 *= j; }
            r16 = fat16;
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        t16 = calcular_ns(start, end);

        // 32 BITS
        clock_gettime(CLOCK_MONOTONIC, &start);
        for(volatile int i = 0; i < ITERATIONS; i++) { 
            uint32_t fat32 = 1;
            for(uint32_t j = 1; j <= val; j++) { fat32 *= j; }
            r32 = fat32;
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        t32 = calcular_ns(start, end);

    } else {
        switch (operador) {
            case '+':
                clock_gettime(CLOCK_MONOTONIC, &start);
                for(volatile int i = 0; i < ITERATIONS; i++) { r4 = (a4 + b4) & 0x0F; }
                clock_gettime(CLOCK_MONOTONIC, &end);
                t4 = calcular_ns(start, end);

                clock_gettime(CLOCK_MONOTONIC, &start);
                for(volatile int i = 0; i < ITERATIONS; i++) { r8 = a8 + b8; }
                clock_gettime(CLOCK_MONOTONIC, &end);
                t8 = calcular_ns(start, end);

                clock_gettime(CLOCK_MONOTONIC, &start);
                for(volatile int i = 0; i < ITERATIONS; i++) { r16 = a16 + b16; }
                clock_gettime(CLOCK_MONOTONIC, &end);
                t16 = calcular_ns(start, end);

                clock_gettime(CLOCK_MONOTONIC, &start);
                for(volatile int i = 0; i < ITERATIONS; i++) { r32 = a32 + b32; }
                clock_gettime(CLOCK_MONOTONIC, &end);
                t32 = calcular_ns(start, end);
                break;

            case '-':
                clock_gettime(CLOCK_MONOTONIC, &start);
                for(volatile int i = 0; i < ITERATIONS; i++) { r4 = (a4 - b4) & 0x0F; }
                clock_gettime(CLOCK_MONOTONIC, &end);
                t4 = calcular_ns(start, end);

                clock_gettime(CLOCK_MONOTONIC, &start);
                for(volatile int i = 0; i < ITERATIONS; i++) { r8 = a8 - b8; }
                clock_gettime(CLOCK_MONOTONIC, &end);
                t8 = calcular_ns(start, end);

                clock_gettime(CLOCK_MONOTONIC, &start);
                for(volatile int i = 0; i < ITERATIONS; i++) { r16 = a16 - b16; }
                clock_gettime(CLOCK_MONOTONIC, &end);
                t16 = calcular_ns(start, end);

                clock_gettime(CLOCK_MONOTONIC, &start);
                for(volatile int i = 0; i < ITERATIONS; i++) { r32 = a32 - b32; }
                clock_gettime(CLOCK_MONOTONIC, &end);
                t32 = calcular_ns(start, end);
                break;

            case '*':
                clock_gettime(CLOCK_MONOTONIC, &start);
                for(volatile int i = 0; i < ITERATIONS; i++) { r4 = (a4 * b4) & 0x0F; }
                clock_gettime(CLOCK_MONOTONIC, &end);
                t4 = calcular_ns(start, end);

                clock_gettime(CLOCK_MONOTONIC, &start);
                for(volatile int i = 0; i < ITERATIONS; i++) { r8 = a8 * b8; }
                clock_gettime(CLOCK_MONOTONIC, &end);
                t8 = calcular_ns(start, end);

                clock_gettime(CLOCK_MONOTONIC, &start);
                for(volatile int i = 0; i < ITERATIONS; i++) { r16 = a16 * b16; }
                clock_gettime(CLOCK_MONOTONIC, &end);
                t16 = calcular_ns(start, end);

                clock_gettime(CLOCK_MONOTONIC, &start);
                for(volatile int i = 0; i < ITERATIONS; i++) { r32 = a32 * b32; }
                clock_gettime(CLOCK_MONOTONIC, &end);
                t32 = calcular_ns(start, end);
                break;

            default:
                printf("Operador inválido!\n");
                return 1;
        }
    }

    printf("\n%-10s | %-12s | %-15s\n", "Largura", "Resultado", "Tempo Total (ms)");
    printf("-----------------------------------------------\n");
    printf("%-10s | %-12u | %-15.2f\n", "4 bits", r4, t4 / 1e6);
    printf("%-10s | %-12u | %-15.2f\n", "8 bits", r8, t8 / 1e6);
    printf("%-10s | %-12u | %-15.2f\n", "16 bits", r16, t16 / 1e6);
    printf("%-10s | %-12u | %-15.2f\n", "32 bits", r32, t32 / 1e6);

    return 0;
}