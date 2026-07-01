#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#define SAMPLES 10
#define ITER_PER_SAMPLE 1000000

// Função auxiliar: Multiplicação por somas sucessivas
uint64_t multiplicar_por_somas(uint64_t a, uint64_t b) {
    uint64_t resultado = 0;
    for (uint64_t i = 0; i < b; i++) {
        resultado += a;
    }
    return resultado;
}

double calcular_ns(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
}

#define BENCHMARK_LOOP(stmt, t_total, t_mean, t_std) \
    do { \
        double sample_times[SAMPLES]; \
        double sum = 0, sq_diff_sum = 0; \
        for(int s = 0; s < SAMPLES; s++) { \
            clock_gettime(CLOCK_MONOTONIC, &start); \
            for(volatile int i = 0; i < ITER_PER_SAMPLE; i++) { stmt; } \
            clock_gettime(CLOCK_MONOTONIC, &end); \
            sample_times[s] = calcular_ns(start, end) / ITER_PER_SAMPLE; \
            sum += sample_times[s]; \
        } \
        t_mean = sum / SAMPLES; \
        for(int s = 0; s < SAMPLES; s++) { \
            sq_diff_sum += (sample_times[s] - t_mean) * (sample_times[s] - t_mean); \
        } \
        t_std = sqrt(sq_diff_sum / (SAMPLES > 1 ? SAMPLES - 1 : 1)); \
        t_total = (sum * ITER_PER_SAMPLE) / 1e6; \
    } while(0)

int main() {
    int64_t entrada1, entrada2 = 0;
    char operador;
    struct timespec start, end;
    double t4_tot, t4_mean, t4_std, t8_tot, t8_mean, t8_std, t16_tot, t16_mean, t16_std, t32_tot, t32_mean, t32_std, t64_tot, t64_mean, t64_std;

    printf("Digite o primeiro número: ");
    if (scanf("%lld", &entrada1) != 1) return 1;
    printf("Digite o operador (+, -, *, !): ");
    if (scanf(" %c", &operador) != 1) return 1;
    if (operador != '!') {
        printf("Digite o segundo número: ");
        if (scanf("%lld", &entrada2) != 1) return 1;
    }

    uint8_t  a4 = entrada1 & 0x0F, b4 = entrada2 & 0x0F, r4 = 0;
    uint8_t  a8 = entrada1, b8 = entrada2, r8 = 0;
    uint16_t a16 = entrada1, b16 = entrada2, r16 = 0;
    uint32_t a32 = entrada1, b32 = entrada2, r32 = 0;
    uint64_t a64 = entrada1, b64 = entrada2, r64 = 0;

    printf("\n=== CALCULADORA BENCHMARK (Multiplicação por somas) ===\n");

    if (operador == '!') {
        uint32_t val = (entrada1 < 0) ? 0 : (uint32_t)entrada1;
        BENCHMARK_LOOP({ uint64_t f = 1; for(uint64_t j=1; j<=val; j++) f = multiplicar_por_somas(f, j) & 0x0F; r4 = f; }, t4_tot, t4_mean, t4_std);
        BENCHMARK_LOOP({ uint64_t f = 1; for(uint64_t j=1; j<=val; j++) f = multiplicar_por_somas(f, j); r8 = f; }, t8_tot, t8_mean, t8_std);
        BENCHMARK_LOOP({ uint64_t f = 1; for(uint64_t j=1; j<=val; j++) f = multiplicar_por_somas(f, j); r16 = f; }, t16_tot, t16_mean, t16_std);
        BENCHMARK_LOOP({ uint64_t f = 1; for(uint64_t j=1; j<=val; j++) f = multiplicar_por_somas(f, j); r32 = f; }, t32_tot, t32_mean, t32_std);
        BENCHMARK_LOOP({ uint64_t f = 1; for(uint64_t j=1; j<=val; j++) f = multiplicar_por_somas(f, j); r64 = f; }, t64_tot, t64_mean, t64_std);
    } else {
        switch (operador) {
            case '+':
                BENCHMARK_LOOP(r4 = (a4 + b4) & 0x0F, t4_tot, t4_mean, t4_std);
                BENCHMARK_LOOP(r8 = a8 + b8, t8_tot, t8_mean, t8_std);
                BENCHMARK_LOOP(r16 = a16 + b16, t16_tot, t16_mean, t16_std);
                BENCHMARK_LOOP(r32 = a32 + b32, t32_tot, t32_mean, t32_std);
                BENCHMARK_LOOP(r64 = a64 + b64, t64_tot, t64_mean, t64_std);
                break;
            case '-':
                BENCHMARK_LOOP(r4 = (a4 - b4) & 0x0F, t4_tot, t4_mean, t4_std);
                BENCHMARK_LOOP(r8 = a8 - b8, t8_tot, t8_mean, t8_std);
                BENCHMARK_LOOP(r16 = a16 - b16, t16_tot, t16_mean, t16_std);
                BENCHMARK_LOOP(r32 = a32 - b32, t32_tot, t32_mean, t32_std);
                BENCHMARK_LOOP(r64 = a64 - b64, t64_tot, t64_mean, t64_std);
                break;
            case '*':
                BENCHMARK_LOOP(r4 = multiplicar_por_somas(a4, b4) & 0x0F, t4_tot, t4_mean, t4_std);
                BENCHMARK_LOOP(r8 = multiplicar_por_somas(a8, b8), t8_tot, t8_mean, t8_std);
                BENCHMARK_LOOP(r16 = multiplicar_por_somas(a16, b16), t16_tot, t16_mean, t16_std);
                BENCHMARK_LOOP(r32 = multiplicar_por_somas(a32, b32), t32_tot, t32_mean, t32_std);
                BENCHMARK_LOOP(r64 = multiplicar_por_somas(a64, b64), t64_tot, t64_mean, t64_std);
                break;
            default:
                printf("Operador inválido!\n");
                return 1;
        }
    }

    printf("\n%-10s | %-20s | %-16s | %-17s | %-15s\n", "Largura", "Resultado", "Total (ms)", "Média/iter (ns)", "Desv Pad (ns)");
    printf("--------------------------------------------------------------------------------------------------\n");
    printf("%-10s | %-20u | %-16.2f | %-17.4f | %-15.4f\n", "4 bits", r4, t4_tot, t4_mean, t4_std);
    printf("%-10s | %-20u | %-16.2f | %-17.4f | %-15.4f\n", "8 bits", r8, t8_tot, t8_mean, t8_std);
    printf("%-10s | %-20u | %-16.2f | %-17.4f | %-15.4f\n", "16 bits", r16, t16_tot, t16_mean, t16_std);
    printf("%-10s | %-20u | %-16.2f | %-17.4f | %-15.4f\n", "32 bits", r32, t32_tot, t32_mean, t32_std);
    printf("%-10s | %-20llu | %-16.2f | %-17.4f | %-15.4f\n", "64 bits", (unsigned long long)r64, t64_tot, t64_mean, t64_std);

    return 0;
}