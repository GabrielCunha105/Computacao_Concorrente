/**
 * @author: Gabriel Rodrigues Cunha - 119143696
 * @date: 13/08/2021
 * 
 * Programa de multiplicação de matrizes geradas aleatóriamente.
 * Utiliza o método tradicional e o de Strassen.
 * 
 * Para compilar e executar:
 * $ gcc main.c Matriz.c -o strassen -lpthread
 * $ ./strassen <N> <Número de Threads> (Dimensão das Matrizes será 2^N X 2^N)
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "Matriz.h"

// Matrizes menores que 128 X 128 serão multiplicadas pelo método tradicional.
#define LIMITE_TRADICIONAL 128

#define GET_TIME(now) { \
   struct timespec time; \
   clock_gettime(CLOCK_MONOTONIC, &time); \
   now = time.tv_sec + time.tv_nsec/1000000000.0; \
}

int n;
int nthreads;

/**
 * Lê os argumentos fornecidos pelo usuário
 */
void ler_args(int argc, char const *argv[]) {
    if (argc < 3) {
        printf("Digite %s <N> <Número de Threads> (Dimensão das Matrizes será 2^N X 2^N)\n", argv[0]);
        exit(1);
    }
    n = atoi(argv[1]);
    nthreads = atoi(argv[2]);
}

/**
 * Cria duas matrizes aleatórias, multiplica pelo método tradicional e o de
 * Strassen e imprime o tempo e o resultado de cada método.
 */
int main(int argc, char const *argv[])
{
    srand(time(0));
    ler_args(argc, argv);

    double inicio, fim;

    // 0x1 << n emula o comportamento de 2^n
    Matriz *a = matriz_aleatoria(0x1 << n, 0x1 << n);
    Matriz *b = matriz_aleatoria(0x1 << n, 0x1 << n);

    printf("Multiplicando matrizes %d X %d ...\n", a->altura, a->altura);
    printf("\nMétodo utilizado:\t\t\tTempo para calcular:\n");

    inicio = clock();
    Matriz *cT = multiplicacao_tradiconal(a, b);
    fim = clock();
    double tempoT = (double) (fim - inicio)/CLOCKS_PER_SEC;
    printf("Multiplicação tradicional\t\t%f Segundos\n", tempoT);

    GET_TIME(inicio);
    Matriz *cSS = multiplicacao_strassen(a, b, LIMITE_TRADICIONAL, 0); 
    GET_TIME(fim);
    double tempoSS = fim - inicio;
    printf("Método de Strassen Sequencial\t\t%f Segundos\n", tempoSS);

    GET_TIME(inicio);
    Matriz *cSC = multiplicacao_strassen(a, b, LIMITE_TRADICIONAL, nthreads);
    GET_TIME(fim);
    double tempoSC = fim - inicio;
    printf("Método de Strassen com %d threads\t%f Segundos\n", nthreads, tempoSC);

    if (comparar(cT, cSS) && comparar(cT, cSC)) {
        printf("\nResultados foram comparados, os três são iguais.\n");
    } else {
        fprintf(stderr, "ERRO--Um resultado diverge dos demais\n");
        exit(2);
    }

    return 0;
}
