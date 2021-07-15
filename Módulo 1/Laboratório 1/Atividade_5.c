/**
 * Módulo 1 - Laboratório 1 - Atividade 5
 * Nome: Gabriel Rodrigues Cunha
 * DRE: 119143696
 * 
 * Este programa cria uma array com ARRLEN elementos onde arr[i] = i e calcula o
 * quadrado de cada um destes elementos dividindo a tarefa da multiplicação entre
 * as NTHREADS threads.
 * 
 * Por exemplo, se ARRLEN = 10000 e NTHREADS = 2, então duas threads serão criadas e
 * a primeira irá calular o quadrado dos primeiros 5000 elementos da lista e
 * a segunda irá calular o quadrado dos últimos 5000.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS 2   // Total de threads a serem criadas
#define ARRLEN 10000 // Comprimento do array cujos elementos serão elevados a 2

// Cria a estrutura de dados para armazenar os argumentos da thread
typedef struct
{
    int *p_arr; // ponteiro do array com os elementos

    int range_start; // A thread multiplicará os elementos no array cujo índice está
    int range_end;   // entre range_start range_end
} t_Args;

// Cácula o quadrado dos elementos dentro do array e do intervalo especificado por args
void *calcularQuadrados(void *arg)
{
    t_Args *args = (t_Args *)arg;
    int i;
    for (i = args->range_start; i < args->range_end; i++)
    {
        args->p_arr[i] = args->p_arr[i] * args->p_arr[i];
    }
    free(arg);
    pthread_exit(NULL);
}

// Inicia a array apontada por arr tal que arr[i] = i
void iniciarArray(int *arr)
{
    for (int i = 0; i < ARRLEN; i++)
    {
        arr[i] = i;
    }
}

// Verifica se todos os elementos da array foram multiplicados corretamente
void verificarQuadarados(int *arr)
{
    for (int i = 0; i < ARRLEN; i++)
    {
        if (arr[i] != i * i)
        {
            printf("Multiplicação falhou. arr[%d] = %d\n", i, arr[i]);
            return;
        }
    }
    printf("Multiplicação bem-sucedida.\n");
}

int main()
{
    pthread_t tid_sistema[NTHREADS]; //identificadores das threads no sistema
    int thread;                      //variavel auxiliar
    t_Args *arg;                     //receberá os argumentos para a thread
    int num_arr[ARRLEN];             // Array com os elementos cujos quadrados serão calculados

    iniciarArray(num_arr);

    for (thread = 0; thread < NTHREADS; thread++)
    {
        arg = malloc(sizeof(t_Args));
        if (arg == NULL)
        {
            printf("--ERRO: malloc()\n");
            exit(-1);
        }

        // Todas as threads recebem um intervalo de mesmo comprimento para
        // realizar as multiplicações
        arg->p_arr = num_arr;
        arg->range_start = ARRLEN * thread / NTHREADS ;
        arg->range_end = ARRLEN * (thread + 1) / NTHREADS;

        printf("--Thread %d multiplicará os elementos entre os índices %d e %d.\n",
            thread, arg->range_start, arg->range_end - 1);
        if (pthread_create(&tid_sistema[thread], NULL, calcularQuadrados, (void *)arg))
        {
            printf("--ERRO: pthread_create()\n");
            exit(-1);
        }
    }

    //--espera todas as threads terminarem
    for (thread = 0; thread < NTHREADS; thread++)
    {
        if (pthread_join(tid_sistema[thread], NULL))
        {
            printf("--ERRO: pthread_join() \n");
            exit(-1);
        }
    }

    verificarQuadarados(num_arr);
    printf("--Thread principal terminou.\n");
}