#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// etapa = 0: thread 2 pode imprimir
// etapa = 1 ou 2: threads 1 e 4 podem imprimir
// etapa = 3: thread 3 pode imprimir
int etapa;

// mutex
pthread_mutex_t etapa_mutex;

// condição que o programa entrou na etapa 1, logo, as threads 1 e 4 podem imprimir
pthread_cond_t etapa_1_cond;

// condição que o programa entrou na etapa 3, logo, a thread 3 pode imprimir
pthread_cond_t etapa_3_cond;

void *thread_1(void *args) {
    pthread_mutex_lock(&etapa_mutex);
    // Não é nescessário while pois o sinal de condição só é enviado uma vez, pela thread 2.
    // Não é nescessário verificar se etapa == 3 pois o programa nunca chegará
    // nesta etapa antes dessa thread imprimir
    if (etapa == 0)
    {
        pthread_cond_wait(&etapa_1_cond, &etapa_mutex);
    }
    pthread_mutex_unlock(&etapa_mutex);

    printf("Fique a vontade.\n");

    pthread_mutex_lock(&etapa_mutex);
    // Verifica se ambas as threads 1 e 4 já imprimiram e, em caso afirmativo,
    // avisa a thread 3 que ela pode imprimir
    if (++etapa == 3)
    {
        pthread_cond_signal(&etapa_3_cond);
    }
    pthread_mutex_unlock(&etapa_mutex);
    return NULL;
}

void *thread_2(void *args) {

    // Não é preciso verificar a etapa pois o programa nunca deixa a etapa 0
    // antes dessa mensagem ser impressa
    printf("Seja bem-vindo!\n");

    pthread_mutex_lock(&etapa_mutex);

    // muda para a etapa 1
    etapa = 1;

    // Avisa as threads 1 e 4 que elas podem imprimir
    pthread_cond_broadcast(&etapa_1_cond);

    pthread_mutex_unlock(&etapa_mutex);

    pthread_exit(NULL);
    return NULL;
}

void *thread_3(void *args) {
    pthread_mutex_lock(&etapa_mutex);
    // Não é nescessário while pois o sinal de condição só é enviado uma vez, pela thread 1 ou 4.
    if (etapa < 3)
    {
        pthread_cond_wait(&etapa_3_cond, &etapa_mutex);
    }
    pthread_mutex_unlock(&etapa_mutex);

    printf("Volte sempre!\n");

    pthread_exit(NULL);
    return NULL;
}

void *thread_4(void *args) {
    pthread_mutex_lock(&etapa_mutex);
    // Não é nescessário while pois o sinal de condição só é enviado uma vez, pela thread 2.
    // Não é nescessário verificar se etapa == 3 pois o programa nunca chegará
    // nesta etapa antes dessa thread imprimir
    if (etapa == 0)
    {
        pthread_cond_wait(&etapa_1_cond, &etapa_mutex);
    }
    pthread_mutex_unlock(&etapa_mutex);

    printf("Sente-se por favor.\n");

    pthread_mutex_lock(&etapa_mutex);
    // Verifica se ambas as threads 1 e 4 já imprimiram e, em caso afirmativo,
    // avisa a thread 3 que ela pode imprimir
    if (++etapa == 3)
    {
        pthread_cond_signal(&etapa_3_cond);
    }
    pthread_mutex_unlock(&etapa_mutex);

    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char const *argv[])
{ 
    pthread_t threads[4];
    etapa = 0;

    // Inicilaiza o mutex (lock de exclusao mutua) e as variaveis de condicao
    pthread_mutex_init(&etapa_mutex, NULL);
    pthread_cond_init (&etapa_1_cond, NULL);
    pthread_cond_init (&etapa_3_cond, NULL);

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

    // Desaloca variaveis e termina
    pthread_mutex_destroy(&etapa_mutex);
    pthread_cond_destroy(&etapa_1_cond);
    pthread_cond_destroy(&etapa_3_cond);
    return 0;
}
