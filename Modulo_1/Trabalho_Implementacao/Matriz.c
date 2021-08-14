/**
 * @author: Gabriel Rodrigues Cunha - 119143696
 * @date: 13/08/2021
 * 
 * Modulo para criação e manipulação de matrizes
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "Matriz.h"

Args_Strassen *tarefas[7];
extern int nthreads;

/**
 * Cria uma nova matriz em branco.
 */
Matriz *iniciar_matriz(int altura, int largura) {
    Matriz *out = malloc(sizeof(Matriz));
    out->inicioX = 0;
    out->inicioY = 0;
    out->altura = altura;
    out->largura = largura;
    out->celulas = malloc(sizeof(int*) * altura);
    for (int i = 0; i < altura; i++) {
        out->celulas[i] = malloc(sizeof(int*) * largura);
    }
    

    return out;
}

/**
 * Cria uma matriz dentro de outra matriz. Os parametros `altura`, `largura`,
 * `inicioY` e `inicioX` determinam qual seção da matriz `from` a matriz `out`
 * deve representar.
 */
void iniciar_submatriz(int altura, int largura, Matriz *from,
        int inicioY, int inicioX, Matriz *out) {
    out->inicioX = from->inicioX + inicioX;
    out->inicioY = from->inicioY + inicioY;
    out->altura = altura;
    out->largura = largura;
    out->celulas = from->celulas;
}

/**
 * Cria um objeto do tipo Args_Strassen. 'a' e 'b' são as matrizes a serem
 * multiplicadas. matrizes menores que 'limiteTradicional' serão multiplicadas
 * pelo algorítmo tradicional de multiplicação.
 */
Args_Strassen *iniciar_Args_Strassen(Matriz *a, Matriz *b, int limiteTradicional) {
    Args_Strassen *out = malloc(sizeof(Args_Strassen));
    out->a = a;
    out->b = b;
    out->ret = NULL; // O resultado da multiplicação será armazenado aqui
    out->limiteTradicional = limiteTradicional;

    return out;
}

/**
 * Libera o espaço de memória ocupado por uma matriz.
 */
void apagar_matriz(Matriz *matriz) {
    for (int i = 0; i < matriz->altura; i++) {
        free(matriz->celulas[i]);
    }
    free(matriz->celulas);
    free(matriz);
}

/**
 * Libera o espaço de memória ocupado por uma submatriz.
 */
void apagar_submatriz(Matriz *submatriz) {
    free(submatriz);
}

void apagar_t_args(Args_Strassen *t_args) {
    free(t_args);
}

/**
 * Cria uma nova matriz com valores aleatórios entre -1000 e 1000.
 */
Matriz *matriz_aleatoria(int altura, int largura) {
    Matriz *out = iniciar_matriz(altura, largura);

    for (int i = 0; i < altura; i++) {
        for (int j =0; j < largura; j++) {
            set_celula(out, i, j, rand()%2001 - 1000);
        }
    }

    return out;
}

/**
 * Retorna o valor na posição (y, x) em `*matriz`.
 */
int get_celula(Matriz *matriz, int y, int x) {
    return matriz->celulas[matriz->inicioY + y][matriz->inicioX + x];
}

/**
 * Atualiza valor na posição (y, x) em `*matriz` para `valor`.
 */
int set_celula(Matriz *matriz, int y, int x, int valor) {
    matriz->celulas[matriz->inicioY + y][matriz->inicioX + x] = valor;
}

/**
 * Multiplica duas matrizes pelo método tradicional de multiplicação.
 */
Matriz *multiplicacao_tradiconal(Matriz *a, Matriz *b) {
    if (a->largura != b->altura) {
        printf("Multiplicação indevida de matrizes\n");
        exit(EXIT_FAILURE);
    }

    Matriz *c = iniciar_matriz(a->altura, b->largura);
    int n = a->largura;

    for (int i = 0; i < c->altura; i++) {
        for (int j = 0; j < c->largura; j++) {
            int *currCelula = &c->celulas[i][j];
            for(int k = 0; k < n; k++) {
                *currCelula += get_celula(a, i, k) * get_celula(b, k, j);
            }
        }
    }
    return c;
}

/**
 * Soma as matrizes apontadas por `*a` e `*b`, e aloca o resultado em `*out`.
 */
void soma_matriz(Matriz *a, Matriz *b, Matriz *out) {
    for(int i = 0; i < out->altura; i++) {
        for(int j = 0; j < out->largura; j++) {
            set_celula(out, i, j, get_celula(a, i, j) + get_celula(b, i, j));
        }
    }
}

/**
 * Subtrai as matrizes apontadas por `*a` e `*b`, e aloca o resultado em `*out`.
 */
void subtrair_matriz(Matriz *a, Matriz *b, Matriz *out) {
    for(int i = 0; i < out->altura; i++) {
        for(int j = 0; j < out->largura; j++) {
            set_celula(out, i, j, get_celula(a, i, j) - get_celula(b, i, j));
        }
    }
}

/**
 * Formata as matrizes em quadrados de lado 2^n e multiplica pelo método de
 * Strassen. Para matrizes menores que `limiteTradicional`, o algoritmo passará
 * a multiplicar pelo método tradiconal de multiplicação.
 */
Matriz *multiplicacao_strassen(Matriz *a, Matriz *b, int limiteTradicional, int concorrente) {
    Matriz *c = (concorrente)?
        strassen_concorrente(a, b, limiteTradicional)
        :
        strassen_sequencial(a, b, limiteTradicional);

    c->altura = a->altura;
    c->largura = b->largura;
    
    return c;
}

/**
 * Quebra a matriz `*a` em quatro setores. os setores gerados serão alocados nos
 * outros 4 ponteiros fornecidos.
 */
void dividir_matriz(Matriz *a, Matriz *a11, Matriz *a12, Matriz *a21, Matriz *a22) {
    int div = a->altura/2;

    iniciar_submatriz(div, div, a, 0, 0, a11);
    iniciar_submatriz(div, div, a, 0, div, a12);
    iniciar_submatriz(div, div, a, div, 0, a21);
    iniciar_submatriz(div, div, a, div, div, a22);
}

/**
 * Função que realiza a multiplicação de Strassen própriamente dita.
 * Assume qua as matrizes `*a` e `*b` já estejam no formato correto. Para maior
 * eficiência, matrizes menores que `limiteTradicional` serão multiplicadas pelo
 * método tradicional de multiplicação
 */
Matriz *strassen_sequencial(Matriz *a, Matriz *b, int limiteTradicional) {
    if (a->altura <= limiteTradicional) {
        return multiplicacao_tradiconal(a, b);
    }
    int tamanhoSub = a->altura/2;

    Matriz *a11 = malloc(sizeof(Matriz));
    Matriz *a12 = malloc(sizeof(Matriz));
    Matriz *a21 = malloc(sizeof(Matriz));
    Matriz *a22 = malloc(sizeof(Matriz));
    dividir_matriz(a, a11, a12, a21, a22);

    Matriz *b11 = malloc(sizeof(Matriz));
    Matriz *b12 = malloc(sizeof(Matriz));
    Matriz *b21 = malloc(sizeof(Matriz));
    Matriz *b22 = malloc(sizeof(Matriz));
    dividir_matriz(b, b11, b12, b21, b22);

    Matriz *out = iniciar_matriz(a->altura, b->largura);
    Matriz *out11 = malloc(sizeof(Matriz));
    Matriz *out12 = malloc(sizeof(Matriz));
    Matriz *out21 = malloc(sizeof(Matriz));
    Matriz *out22 = malloc(sizeof(Matriz));
    dividir_matriz(out, out11, out12, out21, out22);

    Matriz *s1 = iniciar_matriz(tamanhoSub, tamanhoSub);
    subtrair_matriz(b12, b22, s1);
    Matriz *s2 = iniciar_matriz(tamanhoSub, tamanhoSub);
    soma_matriz(a11, a12, s2);
    Matriz *s3 = iniciar_matriz(tamanhoSub, tamanhoSub);
    soma_matriz(a21, a22, s3);
    Matriz *s4 = iniciar_matriz(tamanhoSub, tamanhoSub);
    subtrair_matriz(b21, b11, s4);
    Matriz *s5 = iniciar_matriz(tamanhoSub, tamanhoSub);
    soma_matriz(a11, a22, s5);
    Matriz *s6 = iniciar_matriz(tamanhoSub, tamanhoSub);
    soma_matriz(b11, b22, s6);
    Matriz *s7 = iniciar_matriz(tamanhoSub, tamanhoSub);
    subtrair_matriz(a12, a22, s7);
    Matriz *s8 = iniciar_matriz(tamanhoSub, tamanhoSub);
    soma_matriz(b21, b22, s8);
    Matriz *s9 = iniciar_matriz(tamanhoSub, tamanhoSub);
    subtrair_matriz(a11, a21, s9); 
    Matriz *s10 = iniciar_matriz(tamanhoSub, tamanhoSub);
    soma_matriz(b11, b12, s10);

    Matriz *p1 = strassen_sequencial(a11, s1, limiteTradicional);
    Matriz *p2 = strassen_sequencial(s2, b22, limiteTradicional);
    Matriz *p3 = strassen_sequencial(s3, b11, limiteTradicional);
    Matriz *p4 = strassen_sequencial(a22, s4, limiteTradicional);
    Matriz *p5 = strassen_sequencial(s5, s6, limiteTradicional);
    Matriz *p6 = strassen_sequencial(s7, s8, limiteTradicional);
    Matriz *p7 = strassen_sequencial(s9, s10, limiteTradicional);

    apagar_matriz(s1);
    apagar_matriz(s2);
    apagar_matriz(s3);
    apagar_matriz(s4);
    apagar_matriz(s5);
    apagar_matriz(s6);
    apagar_matriz(s7);
    apagar_matriz(s8);
    apagar_matriz(s9);
    apagar_matriz(s10);

    apagar_submatriz(a11);
    apagar_submatriz(a12);
    apagar_submatriz(a21);
    apagar_submatriz(a22);

    apagar_submatriz(b11);
    apagar_submatriz(b12);
    apagar_submatriz(b21);
    apagar_submatriz(b22);

    Matriz *temp1 = iniciar_matriz(tamanhoSub, tamanhoSub);
    Matriz *temp2 = iniciar_matriz(tamanhoSub, tamanhoSub);

    soma_matriz(p4, p5, temp1);
    subtrair_matriz(p6, p2, temp2);
    soma_matriz(temp1, temp2, out11);
    soma_matriz(p1, p2, out12);
    soma_matriz(p3, p4, out21);
    soma_matriz(p1, p5, temp1);
    soma_matriz(p3, p7, temp2);
    subtrair_matriz(temp1, temp2, out22);

    apagar_matriz(temp1);
    apagar_matriz(temp2);

    apagar_submatriz(out11);
    apagar_submatriz(out12);
    apagar_submatriz(out21);
    apagar_submatriz(out22);

    return out;
}

void *tarefa_thread(void *arg) {
    long id = (long) arg;

    for (int i = id; i < 7; i += nthreads) {
        Args_Strassen *tarefa = tarefas[i];
        tarefa->ret = strassen_sequencial(tarefa->a, tarefa->b, tarefa->limiteTradicional);
    }

    return NULL;
}


/**
 * Função que realiza a multiplicação de Strassen própriamente dita de forma
 * concorrente usando 'nthreads' threads.
 * Assume qua as matrizes `*a` e `*b` já estejam no formato correto. Para maior
 * eficiência, matrizes menores que `limiteTradicional` serão multiplicadas pelo
 * método tradicional de multiplicação
 */
Matriz *strassen_concorrente(Matriz *a, Matriz *b, int limiteTradicional) {

    if (a->altura <= limiteTradicional) {
        return multiplicacao_tradiconal(a, b);
    }
    int tamanhoSub = a->altura/2;
    pthread_t tids[4];


    Matriz *a11 =malloc(sizeof(Matriz));
    Matriz *a12 =malloc(sizeof(Matriz));
    Matriz *a21 =malloc(sizeof(Matriz));
    Matriz *a22 =malloc(sizeof(Matriz));
    dividir_matriz(a, a11, a12, a21, a22);

    Matriz *b11 =malloc(sizeof(Matriz));
    Matriz *b12 =malloc(sizeof(Matriz));
    Matriz *b21 =malloc(sizeof(Matriz));
    Matriz *b22 =malloc(sizeof(Matriz));
    dividir_matriz(b, b11, b12, b21, b22);

    Matriz *out = iniciar_matriz(a->altura, b->largura);
    Matriz *out11 =malloc(sizeof(Matriz));
    Matriz *out12 =malloc(sizeof(Matriz));
    Matriz *out21 =malloc(sizeof(Matriz));
    Matriz *out22 =malloc(sizeof(Matriz));
    dividir_matriz(out, out11, out12, out21, out22);

    Matriz *s1 = iniciar_matriz(tamanhoSub, tamanhoSub);
    subtrair_matriz(b12, b22, s1);
    Matriz *s2 = iniciar_matriz(tamanhoSub, tamanhoSub);
    soma_matriz(a11, a12, s2);
    Matriz *s3 = iniciar_matriz(tamanhoSub, tamanhoSub);
    soma_matriz(a21, a22, s3);
    Matriz *s4 = iniciar_matriz(tamanhoSub, tamanhoSub);
    subtrair_matriz(b21, b11, s4);
    Matriz *s5 = iniciar_matriz(tamanhoSub, tamanhoSub);
    soma_matriz(a11, a22, s5);
    Matriz *s6 = iniciar_matriz(tamanhoSub, tamanhoSub);
    soma_matriz(b11, b22, s6);
    Matriz *s7 = iniciar_matriz(tamanhoSub, tamanhoSub);
    subtrair_matriz(a12, a22, s7);
    Matriz *s8 = iniciar_matriz(tamanhoSub, tamanhoSub);
    soma_matriz(b21, b22, s8);
    Matriz *s9 = iniciar_matriz(tamanhoSub, tamanhoSub);
    subtrair_matriz(a11, a21, s9); 
    Matriz *s10 = iniciar_matriz(tamanhoSub, tamanhoSub);
    soma_matriz(b11, b12, s10);


    tarefas[0] = iniciar_Args_Strassen(a11, s1, limiteTradicional);
    tarefas[1] = iniciar_Args_Strassen(s2, b22, limiteTradicional);
    tarefas[2] = iniciar_Args_Strassen(s3, b11, limiteTradicional);
    tarefas[3] = iniciar_Args_Strassen(a22, s4, limiteTradicional);
    tarefas[4] = iniciar_Args_Strassen(s5, s6, limiteTradicional);
    tarefas[5] = iniciar_Args_Strassen(s7, s8, limiteTradicional);
    tarefas[6] = iniciar_Args_Strassen(s9, s10, limiteTradicional);

    for (long i = 0; i < nthreads; i++) {
        if(pthread_create(&tids[i], NULL, tarefa_thread, (void *) i)) {
            fprintf(stderr, "ERRO--pthread_create\n");
            exit(3);
        }
    }
    for (long i = 0; i < nthreads; i++) {
        pthread_join(tids[i], NULL);
    }

    apagar_matriz(s1);
    apagar_matriz(s2);
    apagar_matriz(s3);
    apagar_matriz(s4);
    apagar_matriz(s5);
    apagar_matriz(s6);
    apagar_matriz(s7);
    apagar_matriz(s8);
    apagar_matriz(s9);
    apagar_matriz(s10);

    apagar_submatriz(a11);
    apagar_submatriz(a12);
    apagar_submatriz(a21);
    apagar_submatriz(a22);

    apagar_submatriz(b11);
    apagar_submatriz(b12);
    apagar_submatriz(b21);
    apagar_submatriz(b22);

    Matriz *temp1 = iniciar_matriz(tamanhoSub, tamanhoSub);
    Matriz *temp2 = iniciar_matriz(tamanhoSub, tamanhoSub);

    soma_matriz(tarefas[3]->ret, tarefas[4]->ret, temp1);
    subtrair_matriz(tarefas[5]->ret, tarefas[1]->ret, temp2);
    soma_matriz(temp1, temp2, out11);
    soma_matriz(tarefas[0]->ret, tarefas[1]->ret, out12);
    soma_matriz(tarefas[2]->ret, tarefas[3]->ret, out21);
    soma_matriz(tarefas[0]->ret, tarefas[4]->ret, temp1);
    soma_matriz(tarefas[2]->ret, tarefas[6]->ret, temp2);
    subtrair_matriz(temp1, temp2, out22);

    apagar_matriz(temp1);
    apagar_matriz(temp2);

    apagar_submatriz(out11);
    apagar_submatriz(out12);
    apagar_submatriz(out21);
    apagar_submatriz(out22);

    return out;
}

/**
 * Comapara as duas matrizes fornecidas. Retorna 1 se elas forem iguais, e 0 se
 * forem diferentes.
 */
int comparar(Matriz *a, Matriz *b) {
    for(int i = 0; i < a->altura; i++) {
        for(int j = 0; j < a->largura; j++) {
            int celA = get_celula(a, i, j);
            int celB = get_celula(b, i, j);

            if (!(celA == celB)) return 0;
        }
    }

    return 1;
}