#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// Tamanho do buffer
#define TAM_BUFFER 5

int buffer[TAM_BUFFER];
long fim;
sem_t semConsumidor;
sem_t semProdutor;

// Número de threads produtoras
int numProdutores;

// Número de threads consumidoras
int numConsumidores;

// Função executada pelas threads produtoras
void *produtor(void *args) {
    const long id = (long) args;

    for (int i = 0; 1; ++i) {
        // Espera o o sinal de produtores
        sem_wait(&semProdutor);

        // Insere o valor de i no buffer
        buffer[fim++] = i;
        printf("Produtor %ld inseriu %d\n", id, i);

        if (fim == TAM_BUFFER) {

            // Se o buffer estiver cheio, permite um consumidor executar e mantém
            // o semáforo de produtores bloqueado
            sem_post(&semConsumidor);

        } else {

            // Se o buffer ainda não estiver cheio, deixa outro produtor executar
            sem_post(&semProdutor);
        }

        // Espera artificial de 1 segundo
        sleep(1);
    }
}

// Função executada pelas threads produtoras
void *consumidor(void *args) {
    const long id = (long) args;

    while (1) {
        // Espera o o sinal de consumidores
        sem_wait(&semConsumidor);

        // Consome todo o buffer
        printf("Consumidor %ld:", id);
        for (int i = 0; i < TAM_BUFFER; ++i) {
            printf(" %d", buffer[i]);
        }
        printf("\n");
        fim = 0;
        
        // Libera as threads produtoras
        sem_post(&semProdutor);

        // Espera artificial de 1 segundo
        sleep(1);
    }
}

// Lê os argumentos fornecidos pelo usuário
void ler_args(int argc, char const *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Digite %s <Número de produtores> <Número de produtores>\n", argv[0]);
        exit(1);
    }
    numProdutores = atoi(argv[1]);
    numConsumidores = atoi(argv[2]);

    return;
}

int main(int argc, char const *argv[])
{
    ler_args(argc, argv);

    // Inicia as variáveis globais
    sem_init(&semProdutor, 0, 1);
    sem_init(&semConsumidor, 0, 0);
    fim = 0;

    pthread_t tids[numProdutores+numConsumidores];

    // Cria as threads produtoras
    for (long i = 0; i < numProdutores; ++i) {
        if(pthread_create(&tids[i], NULL, produtor, (void *) i)) {
            printf("ERRO--pthread_create\n");
            exit(3);
        }
    }

    // Cria as threads consumidoras
    for (long i = 0; i < numConsumidores; ++i) {
        if(pthread_create(&tids[numProdutores+i], NULL, consumidor, (void *) i)) {
            printf("ERRO--pthread_create\n");
            exit(3);
        }
    }

    // Espera as threads
    for (int i = 0; i < numProdutores+numConsumidores; ++i) {
        pthread_join(tids[i], NULL);
    }

    // Destroi os semáforos
    sem_destroy(&semProdutor);
    sem_destroy(&semConsumidor);

    return 0;
}
