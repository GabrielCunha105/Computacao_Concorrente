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

typedef struct Args_Strassen {
    Matriz *a;
    Matriz *b;
    Matriz *ret;
    int limiteTradicional;
} Args_Strassen;

Matriz *iniciar_matriz(int, int);

void iniciar_submatriz(int, int, Matriz*, int, int, Matriz*);

Args_Strassen *iniciar_Args_Strassen(Matriz*, Matriz*, int);

void apagar_matriz(Matriz*);

void apagar_submatriz(Matriz*);

void apagar_t_args(Args_Strassen*);

Matriz *matriz_aleatoria(int, int);

int get_celula(Matriz*, int, int);

int set_celula(Matriz*, int, int, int);

Matriz *multiplicacao_tradiconal(Matriz*, Matriz*);

void soma_matriz(Matriz*, Matriz*, Matriz*);

void subtrair_matriz(Matriz*, Matriz*, Matriz*);

Matriz *multiplicacao_strassen(Matriz*, Matriz*, int, int);

void dividir_matriz(Matriz*, Matriz*, Matriz*, Matriz*, Matriz*);

Matriz *strassen_sequencial(Matriz*, Matriz*, int);

void *tarefa_thread(void*);

Matriz *strassen_concorrente(Matriz*, Matriz*, int);

int comparar(Matriz*, Matriz*);

# endif