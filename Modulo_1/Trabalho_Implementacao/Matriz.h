#ifndef __MATRIZ_H__
#define __MATRIZ_H__

typedef struct Matriz
{
    int **celulas;
    int largura;
    int altura;
    int inicioX;
    int inicioY;
} Matriz;

// Parâmetros nescessários para aplicar o Algorítmo de Strassen.
typedef struct Args_Strassen {
    Matriz *a;
    Matriz *b;
    Matriz *ret;
    int limiteTradicional;
} Args_Strassen;

// Parâmetros recebidos por uma thread
typedef struct Thread_Args {
    Args_Strassen **tarefas;
    int id;
    int nthreads;
} Thread_Args;

Matriz *iniciar_matriz(int, int);

void iniciar_submatriz(int, int, Matriz*, int, int, Matriz*);

Args_Strassen *iniciar_Args_Strassen(Matriz*, Matriz*, int);

void apagar_matriz(Matriz*);

void apagar_submatriz(Matriz*);

void apagar_Args_Strassen(Args_Strassen*);

Matriz *matriz_aleatoria(int, int);

int get_celula(Matriz*, int, int);

int set_celula(Matriz*, int, int, int);

Matriz *multiplicacao_tradiconal(Matriz*, Matriz*);

void soma_matriz(Matriz*, Matriz*, Matriz*);

void subtrair_matriz(Matriz*, Matriz*, Matriz*);

Matriz *multiplicacao_strassen(Matriz*, Matriz*, int, int);

void dividir_matriz(Matriz*, Matriz*, Matriz*, Matriz*, Matriz*);

Matriz *strassen_sequencial(Matriz*, Matriz*, int);

int num_threads_restantes(int, int);

void *tarefa_thread(void*);

Matriz *strassen_concorrente(Matriz*, Matriz*, int, int);

int comparar(Matriz*, Matriz*);

# endif