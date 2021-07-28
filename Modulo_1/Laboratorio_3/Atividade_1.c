#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define NUM_EXECS 5
#define GET_TIME(now) { \
   struct timespec time; \
   clock_gettime(CLOCK_MONOTONIC, &time); \
   now = time.tv_sec + time.tv_nsec/1000000000.0; \
}

int nthreads;
long long int N;
float *vet;
float maxSeq;
float minSeq;
float maxConc;
float minConc;

void ler_args(int argc, char const *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Digite %s <Dimensão do vetor> <Número de Threads>\n", argv[0]);
        exit(1);
    }
    N = atoll(argv[1]);
    nthreads = atoi(argv[2]);

    if (nthreads > N) {
        nthreads = N;
    }
}

void iniciar_variaveis() {
    vet = malloc(sizeof(float)*N);    
    if (vet == NULL) {
        fprintf(stderr, "ERRO--malloc\n");
        exit(2);
    }

    for (int i = 0; i < N; i++) {
        vet[i] = (float) rand()/RAND_MAX;
    }
    maxSeq = vet[0];
    minSeq = vet[0];
    maxConc = vet[0];
    minConc = vet[0];
}

// Cada thread calcula o i-ésimo item de vet, onde i  = id + nthreads*k e
// k é um inteiro positivo qualquer
void * tarefa_thread(void * arg) {
    int id = (long int) arg;

    for (int i = id; i < N; i += nthreads) {
        if (vet[i] < minConc) minConc = vet[i];
        else if (vet[i] > maxConc) maxConc = vet[i];
    }

    pthread_exit(NULL);
    return NULL;
}

void min_max_squencial() {
    for (int i = 0; i < N; i++) {
        if (vet[i] < minSeq) minSeq = vet[i];
        else if (vet[i] > maxSeq) maxSeq = vet[i];
    }
}

void min_max_concorrente() {
    pthread_t tids[nthreads];

    for (long int i = 0; i < nthreads; i++) {
        if(pthread_create(&tids[i], NULL, tarefa_thread, (void *) i)) {
            fprintf(stderr, "ERRO--pthread_create\n");
            exit(3);
        }
    }
    for (int i = 0; i < nthreads; i++) {
        pthread_join(tids[i], NULL);
    }
}

void verificar_min_max() {
    if (minSeq != minConc || maxSeq != maxConc) {
        fprintf(stderr, "ERRO--Mínimos e Máximos não conferem\n");
        exit(4);
    }
}

void liberar_memoria() {
    free(vet);
}

int main(int argc, char const *argv[])
{
    double inicio, fim, delta, totalSeq = 0, totalConc = 0;

    GET_TIME(inicio);
    ler_args(argc, argv);
    iniciar_variaveis();
    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo de inicializacao:\t%lfs\n", delta);

    printf("\n");
    for (int i = 1; i <= NUM_EXECS; i++) {
        GET_TIME(inicio);
        min_max_squencial();
        GET_TIME(fim);
        delta = fim - inicio;
        totalSeq += delta;
        printf("Busca sequencial %d:\t%lfs\n", i, delta);

        GET_TIME(inicio);
        min_max_concorrente();
        GET_TIME(fim);
        delta = fim - inicio;
        totalConc += delta;
        printf("Busca concorrente %d:\t%lfs\n", i, delta);

        printf("\n");
        verificar_min_max();
        maxSeq = vet[0];
        minSeq = vet[0];
        maxConc = vet[0];
        minConc = vet[0];
    }

    printf("Média sequencial:\t%lfs\n", totalSeq/NUM_EXECS);
    printf("Média concorrente:\t%lfs\n", totalConc/NUM_EXECS);
    printf("Aceleração:\t\t%.2lf\n\n", totalSeq/totalConc);

    GET_TIME(inicio);
    liberar_memoria();
    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo de finalizacao:\t%lfs\n", delta);
    
    printf("Busca realizada com sucesso.\n");
    return 0;
}
