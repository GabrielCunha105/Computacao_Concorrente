#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int N;
int *vet;
int bloqueadas = 0;
int etapa = 2;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;

void barreira(int nthreads, long id) {
    pthread_mutex_lock(&x_mutex); //inicio secao critica
    if (bloqueadas == (nthreads-1)) { 
      //ultima thread a chegar na barreira

      printf("Thread %ld liberou a barreira\n\n", id);
      if (++etapa <= N*2 + 1)
        printf("-- Iteração %d | Etapa %d --\n\n", etapa/2, etapa % 2 + 1);

      pthread_cond_broadcast(&x_cond);
      bloqueadas=0;
    } else {
      bloqueadas++;
      printf("Thread %ld chegou na barreira\n", id);
      pthread_cond_wait(&x_cond, &x_mutex);
    }
    pthread_mutex_unlock(&x_mutex); //fim secao critica
}

void ler_args(int argc, char const *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Digite %s <N>\n", argv[0]);
        exit(1);
    }
    N = atoll(argv[1]);

    return;
}

// Retorna um número aleatório entre 0 e 9
int rand_num() {
    return rand() % 10;
}

void iniciar_vars() {
    srand(time(NULL));

    vet = malloc(sizeof(int) * N);
    if (vet == NULL) {
        fprintf(stderr, "ERRO--malloc\n");
        exit(2);
    }
    for (int i = 0; i < N; i++){
        vet[i] = rand_num();
    }

    pthread_mutex_init(&x_mutex, NULL);
    pthread_cond_init(&x_cond, NULL);

    return;
}

void apagar_vars(int **somatorios) {
    free(vet);
    pthread_mutex_destroy(&x_mutex);
    pthread_cond_destroy(&x_cond);
    for (int i = 0; i < N; i++) {
        free(somatorios[i]);
    }

    return;
}

// Imprime o vetor em seu estado atual
void imprimir_vet() {
    printf("[ %d", vet[0]);
    for (int i = 1; i < N; i++) printf(", %d", vet[i]);
    printf(" ]\n");
    return;
}

void *tarefa_thread(void *args) {
    long id = (long) args;
    int *somatorio = malloc(sizeof(int));
    if (somatorio == NULL) {
        fprintf(stderr, "ERRO--malloc\n");
        exit(2);
    }

    for (int i = 0; i < N; i++) {
        printf("Thread %ld começou a calcular o somatório\n", id);
        for (int j = 0; j < N; j++) *somatorio += vet[j];
        printf("Thread %ld calculou o somatório: %d\n", id, *somatorio);
        barreira(N, id);
        printf("Thread %ld começou a alterar o vetor\n", id);
        vet[id] = rand_num();
        printf("Thread %ld alterou o vetor: vet[%ld] = %d\n", id, id, vet[id]);
        barreira(N, id);
    }

    printf("Thread %ld Terminou\n", id);
    pthread_exit(somatorio);

    return NULL;
}

int verificar_resultado(int **somatorios) {
    for (int i = 1; i < N; i++) {
        if (*(somatorios[i]) != *(somatorios[0])) return 0;
    }
    return 1;
}

int main(int argc, char const *argv[])
{
    ler_args(argc, argv);
    iniciar_vars();

    pthread_t tids[N];
    int *somatorios[N];

    printf("Valores Iniciais:\n");
    printf("N = %d\n", N);
    printf("Vetor = ");
    imprimir_vet();

    printf("\n-- Iteração 1 | Etapa 1 --\n\n");
    for (long i = 0; i < N; i++) {
        if(pthread_create(&tids[i], NULL, tarefa_thread, (void *) i)) {
            printf("ERRO--pthread_create\n");
            exit(3);
        }
    }
    for (int i = 0; i < N; i++) {
        pthread_join(tids[i], (void **) &somatorios[i]);
    }
    printf("\n-- Resultado final --\n\n");
    printf("Vetor = ");
    imprimir_vet();
    for (int i = 0; i < N; i++) {
        printf("Somatório da Thread %d:\t%d\n", i, *(somatorios[i]));
    }
    int iguais =(verificar_resultado(somatorios));
    printf("Somatórios são iguais?\t%s\n", (iguais)? "Sim" : "Não");

    apagar_vars(somatorios);
    return 0;
}
