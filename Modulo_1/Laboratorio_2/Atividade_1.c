#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define GET_TIME(now) { \
   struct timespec time; \
   clock_gettime(CLOCK_MONOTONIC, &time); \
   now = time.tv_sec + time.tv_nsec/1000000000.0; \
}

int nthreads;
int dim;
float *matrizA;
float *matrizB;
float *matrizRes;

void ler_args(int argc, char const *argv[]) {
    if (argc < 3) {
        printf("Digite %s <Dimensão das Matrizes> <Número de Threads>\n", argv[0]);
        exit(1);
    }
    dim = atoi(argv[1]);
    nthreads = atoi(argv[2]);

    if (nthreads > dim) {
        nthreads = dim;
    }
}

void alocar_matrizes() {
    matrizA = (float *) malloc(sizeof(float) * dim * dim);
    matrizB = (float *) malloc(sizeof(float) * dim * dim);
    matrizRes = (float *) malloc(sizeof(float) * dim * dim);
    
    if (!(matrizA && matrizB && matrizRes)) {
        printf("ERRO--malloc\n");
        exit(2);
    }
}

void inicializar_matrizes() {
    for (int i = 0; i < dim; i++) {
        for(int j = 0; j < dim; j++) {
            matrizA[i*dim + j] = 1;
            matrizB[i*dim + j] = 1;
            matrizRes[i*dim + j] = 0;
        }
    }
}

// Calcula os valores da i-ésima linha de matrizRes
void multiplicar_linha(int i) {
    for (int j = 0; j < dim; j++) {
        for (int k =0; k < dim; k++) {
            matrizRes[i*dim + j] += matrizA[i*dim + k] * matrizB[k*dim + j];
        }
    }
}

// Cada thread calcula a linha i da matrizRes, onde i  = id + nthreads*k e
// k é um inteiro positivo qualquer
void * tarefa_thread(void * arg) {
    int id = *(int*) arg;

    for (int i = id; i < dim; i += nthreads) {
        multiplicar_linha(i);
    }
    pthread_exit(NULL);
    return NULL;
}

void multiplciar_matrizes() {
    pthread_t tids[nthreads];
    int ids[nthreads];

    for (int i = 0; i < nthreads; i++) {
        ids[i] = i;
        if(pthread_create(&tids[i], NULL, tarefa_thread, (void *) &ids[i])) {
            printf("ERRO--pthread_create\n");
            exit(3);
        }
    }
    for (int i = 0; i < nthreads; i++) {
        pthread_join(tids[i], NULL);
    }
}

void verificar_multiplicacao() {
    for (int i = 0; i < dim; i++) {
        for(int j = 0; j < dim; j++) {
            if( matrizRes[i*dim + j] != dim) {
                printf("ERRO--Multiplicacao invalida\n");
                exit(4);
            }
        }
    }
}

void liberar_memoria() {
    free(matrizA);
    free(matrizB);
    free(matrizRes);
}

int main(int argc, char const *argv[])
{
    double inicio, fim, delta, total;
    total = 0;

    GET_TIME(inicio);
    ler_args(argc, argv);
    alocar_matrizes();
    inicializar_matrizes();
    GET_TIME(fim);
    delta = fim - inicio;
    total += delta;
    printf("Tempo de inicializacao:\t%lfs\n", delta);
    
    GET_TIME(inicio);
    multiplciar_matrizes();
    GET_TIME(fim);
    delta = fim - inicio;
    total += delta;
    verificar_multiplicacao();
    printf("Tempo de multiplicacao:\t%lfs\n", delta);

    GET_TIME(inicio);
    liberar_memoria();
    GET_TIME(fim);
    delta = fim - inicio;
    total += delta;
    printf("Tempo de finalizacao:\t%lfs\n", delta);

    printf("Tempo total:\t\t%lfs\n", total);
    printf("Multiplicacao realizada com sucesso.\n");
    return 0;
}