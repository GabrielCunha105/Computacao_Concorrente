#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

// Etapa 1: Apenas a thread 2 pode imprimir
// Etapa 2: As threads 1 e 4 podem imprimir
// Etapa 3: Apenas a thread 3 pode imprimir

// Sinal da etapa 2
sem_t etapa_2;

// Sinal da etapa 3
sem_t etapa_3;

void *thread_1(void *args) {

    // Espera ser liberada pela thread 2
    sem_wait(&etapa_2);

    // Imprime a mensagem 
    printf("Fique a vontade.\n");

    // Libera a thread 3
    sem_post(&etapa_3);

    pthread_exit(NULL);
    return NULL;
}

void *thread_2(void *args) {
    // Não é preciso fazer nenhuma verificação inicial pois esta é
    // a primeira thread a imprimir

    // Imprime a mensagem 
    printf("Seja bem-vindo!\n");

    // Libera as threads 1 e 4;
    sem_post(&etapa_2);
    sem_post(&etapa_2);

    pthread_exit(NULL);
    return NULL;
}

void *thread_3(void *args) {

    // Espera ser liberada pela thread 1 e pela thread 4
    sem_wait(&etapa_3);
    sem_wait(&etapa_3);

    // Imprime a mensagem 
    printf("Volte sempre!\n");

    pthread_exit(NULL);
    return NULL;
}

void *thread_4(void *args) {


    // Espera ser liberada pela thread 2
    sem_wait(&etapa_2);

    printf("Sente-se por favor.\n");

    // Libera a thread 3
    sem_post(&etapa_3);

    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char const *argv[])
{ 
    pthread_t threads[4];

    // Inicilaiza os semáforos das etapas
    sem_init(&etapa_2, 0, 0);
    sem_init(&etapa_3, 0, 0);

    // Cria as threads
    pthread_create(&threads[0], NULL, thread_1, NULL);
    pthread_create(&threads[1], NULL, thread_2, NULL);
    pthread_create(&threads[2], NULL, thread_3, NULL);
    pthread_create(&threads[3], NULL, thread_4, NULL);

    // Espera todas as threads completarem
    for (int i = 0; i < 4; i++) 
    {
        pthread_join(threads[i], NULL);
    }

    // Destroi os semáforos e termina
    sem_destroy(&etapa_2);
    sem_destroy(&etapa_3);
    
    return 0;
}
