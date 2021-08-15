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

/**
 * Cria uma nova matriz em branco.
 */
Matriz *iniciar_matriz(int altura, int largura) {
    Matriz *out = malloc(sizeof(Matriz));
    if(out == NULL) {
        fprintf(stderr, "ERRO--malloc\n");
        exit(2);
    }

    out->inicioX = 0;
    out->inicioY = 0;
    out->altura = altura;
    out->largura = largura;
    out->celulas = malloc(sizeof(int*) * altura);
    if(out->celulas == NULL) {
        fprintf(stderr, "ERRO--malloc\n");
        exit(2);
    }
    for (int i = 0; i < altura; i++) {
        out->celulas[i] = malloc(sizeof(int*) * largura);
        if(out->celulas[i] == NULL) {
            fprintf(stderr, "ERRO--malloc\n");
            exit(2);
        }
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
    if(out == NULL) {
        fprintf(stderr, "ERRO--malloc\n");
        exit(2);
    }

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

void apagar_Args_Strassen(Args_Strassen *t_args) {
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
Matriz *multiplicacao_strassen(Matriz *a, Matriz *b, int limiteTradicional, int nthreads) {
    Matriz *c = (nthreads)?
        strassen_concorrente(a, b, limiteTradicional, nthreads)
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

    // Divide 'a' em quatro submatrizes
    Matriz *a11 = malloc(sizeof(Matriz));
    Matriz *a12 = malloc(sizeof(Matriz));
    Matriz *a21 = malloc(sizeof(Matriz));
    Matriz *a22 = malloc(sizeof(Matriz));
    if(!(a11 && a12 && a21 && a22)) {
        fprintf(stderr, "ERRO--malloc\n");
        exit(3);
    }
    dividir_matriz(a, a11, a12, a21, a22);

    // Divide 'b' em quatro submatrizes
    Matriz *b11 = malloc(sizeof(Matriz));
    Matriz *b12 = malloc(sizeof(Matriz));
    Matriz *b21 = malloc(sizeof(Matriz));
    Matriz *b22 = malloc(sizeof(Matriz));
    if(!(b11 && b12 && b21 && b22)) {
        fprintf(stderr, "ERRO--malloc\n");
        exit(3);
    }
    dividir_matriz(b, b11, b12, b21, b22);

    // Divide a matriz de saida em quatro submatrizes
    Matriz *out = iniciar_matriz(a->altura, b->largura);
    Matriz *out11 = malloc(sizeof(Matriz));
    Matriz *out12 = malloc(sizeof(Matriz));
    Matriz *out21 = malloc(sizeof(Matriz));
    Matriz *out22 = malloc(sizeof(Matriz));
    if(!(out11 && out12 && out21 && out22)) {
        fprintf(stderr, "ERRO--malloc\n");
        exit(3);
    }
    dividir_matriz(out, out11, out12, out21, out22);

    Matriz *s1 = iniciar_matriz(tamanhoSub, tamanhoSub);
    subtrair_matriz(b12, b22, s1);  // s1 = b12 - b22
    Matriz *s2 = iniciar_matriz(tamanhoSub, tamanhoSub);
    soma_matriz(a11, a12, s2);      // s2 = a11 + a12
    Matriz *s3 = iniciar_matriz(tamanhoSub, tamanhoSub);
    soma_matriz(a21, a22, s3);      // s3 = a21 + a22
    Matriz *s4 = iniciar_matriz(tamanhoSub, tamanhoSub);
    subtrair_matriz(b21, b11, s4);  // s4 = b21 - b11
    Matriz *s5 = iniciar_matriz(tamanhoSub, tamanhoSub);
    soma_matriz(a11, a22, s5);      // s5 = a11 + a22
    Matriz *s6 = iniciar_matriz(tamanhoSub, tamanhoSub);
    soma_matriz(b11, b22, s6);      // s6 = b11 + b22
    Matriz *s7 = iniciar_matriz(tamanhoSub, tamanhoSub);
    subtrair_matriz(a12, a22, s7);  // s7 = a12 - a22
    Matriz *s8 = iniciar_matriz(tamanhoSub, tamanhoSub);
    soma_matriz(b21, b22, s8);      // s8 = b21 + b22
    Matriz *s9 = iniciar_matriz(tamanhoSub, tamanhoSub);
    subtrair_matriz(a11, a21, s9);  // s9 = a11 - a21
    Matriz *s10 = iniciar_matriz(tamanhoSub, tamanhoSub);
    soma_matriz(b11, b12, s10);     // s10 = b11 + b12

    // Executa a recursão do Algortimo de Strassen de forma sequencial
    Matriz *p1 = strassen_sequencial(a11, s1, limiteTradicional); // p0 = a11 * s1
    Matriz *p2 = strassen_sequencial(s2, b22, limiteTradicional); // p1 = s2 * b22
    Matriz *p3 = strassen_sequencial(s3, b11, limiteTradicional); // p2 = s3 * b11
    Matriz *p4 = strassen_sequencial(a22, s4, limiteTradicional); // p3 = a22 * s4
    Matriz *p5 = strassen_sequencial(s5, s6, limiteTradicional);  // p4 = s5 * s6
    Matriz *p6 = strassen_sequencial(s7, s8, limiteTradicional);  // p5 = s7 * s8
    Matriz *p7 = strassen_sequencial(s9, s10, limiteTradicional); // p6 = s9 * s10

    // Limpa as matrizes não mais necessárias da memória
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

    soma_matriz(p4, p5, temp1);           // temp1 = p4 + p5
    subtrair_matriz(p6, p2, temp2);       // temp2 = p6 - p2
    soma_matriz(temp1, temp2, out11);     // out11 = p4 + p5 + p6 - p2
    soma_matriz(p1, p2, out12);           // out12 = p1 + p2
    soma_matriz(p3, p4, out21);           // out21 = p3 + p4
    soma_matriz(p1, p5, temp1);           // temp1 = p1 + p5
    soma_matriz(p3, p7, temp2);           // temp2 = p3 + p7
    subtrair_matriz(temp1, temp2, out22); // out22 = p1 + p5 - p3 - p7

    // Limpa as matrizes não mais necessárias da memória
    apagar_matriz(temp1);
    apagar_matriz(temp2);
    apagar_submatriz(out11);
    apagar_submatriz(out12);
    apagar_submatriz(out21);
    apagar_submatriz(out22);

    return out;
}

/**
 * No algoritmo de Strassen só é possível paralelizar a recursão em 7 threads.
 * Caso o usuário tenha disponibilizado mais que 7 threads para o programa, dado
 * um índice 'i' e o total de threads disponíveis 'nthreads' essa função determina
 * como as threads remanescentes serão distribuidas, de forma homogênea,
 * pelos níveis mais baixos da recursão.
 */
int num_threads_restantes(int i, int nthreads) {
    int restantes = nthreads - 7; // descosidera as threads já usadas pelo nível de recursão atual
    if (restantes <= 0) return 0;

    int resto = restantes % 7;
    restantes /= 7;

    if (i < resto) restantes++;

    return restantes;
}

/**
 * Função executada por cada uma das threads que são criadas
 */
void *tarefa_thread(void *arg) {
    Thread_Args *args = (Thread_Args *) arg;

    for (int i = args->id; i < 7; i += args->nthreads) {
        Args_Strassen *tarefa = args->tarefas[i];
        int restantes = num_threads_restantes(i, args->nthreads);
        tarefa->ret = (restantes > 0)?
        strassen_concorrente(tarefa->a, tarefa->b, tarefa->limiteTradicional, restantes)
        :
        strassen_sequencial(tarefa->a, tarefa->b, tarefa->limiteTradicional);
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
Matriz *strassen_concorrente(Matriz *a, Matriz *b, int limiteTradicional, int nthreads) {
    if (a->altura <= limiteTradicional) {
        return multiplicacao_tradiconal(a, b);
    }
    int tamanhoSub = a->altura/2;
    pthread_t tids[nthreads];

    // Divide 'a' em quatro submatrizes
    Matriz *a11 = malloc(sizeof(Matriz));
    Matriz *a12 = malloc(sizeof(Matriz));
    Matriz *a21 = malloc(sizeof(Matriz));
    Matriz *a22 = malloc(sizeof(Matriz));
    if(!(a11 && a12 && a21 && a22)) {
        fprintf(stderr, "ERRO--malloc\n");
        exit(3);
    }
    dividir_matriz(a, a11, a12, a21, a22);

    // Divide 'b' em quatro submatrizes
    Matriz *b11 = malloc(sizeof(Matriz));
    Matriz *b12 = malloc(sizeof(Matriz));
    Matriz *b21 = malloc(sizeof(Matriz));
    Matriz *b22 = malloc(sizeof(Matriz));
    if(!(b11 && b12 && b21 && b22)) {
        fprintf(stderr, "ERRO--malloc\n");
        exit(3);
    }
    dividir_matriz(b, b11, b12, b21, b22);

    // Divide a matriz de saida em quatro submatrizes
    Matriz *out = iniciar_matriz(a->altura, b->largura);
    Matriz *out11 = malloc(sizeof(Matriz));
    Matriz *out12 = malloc(sizeof(Matriz));
    Matriz *out21 = malloc(sizeof(Matriz));
    Matriz *out22 = malloc(sizeof(Matriz));
    if(!(out11 && out12 && out21 && out22)) {
        fprintf(stderr, "ERRO--malloc\n");
        exit(3);
    }
    dividir_matriz(out, out11, out12, out21, out22);

    Matriz *s1 = iniciar_matriz(tamanhoSub, tamanhoSub);
    subtrair_matriz(b12, b22, s1);  // s1 = b12 - b22
    Matriz *s2 = iniciar_matriz(tamanhoSub, tamanhoSub);
    soma_matriz(a11, a12, s2);      // s2 = a11 + a12
    Matriz *s3 = iniciar_matriz(tamanhoSub, tamanhoSub);
    soma_matriz(a21, a22, s3);      // s3 = a21 + a22
    Matriz *s4 = iniciar_matriz(tamanhoSub, tamanhoSub);
    subtrair_matriz(b21, b11, s4);  // s4 = b21 - b11
    Matriz *s5 = iniciar_matriz(tamanhoSub, tamanhoSub);
    soma_matriz(a11, a22, s5);      // s5 = a11 + a22
    Matriz *s6 = iniciar_matriz(tamanhoSub, tamanhoSub);
    soma_matriz(b11, b22, s6);      // s6 = b11 + b22
    Matriz *s7 = iniciar_matriz(tamanhoSub, tamanhoSub);
    subtrair_matriz(a12, a22, s7);  // s7 = a12 - a22
    Matriz *s8 = iniciar_matriz(tamanhoSub, tamanhoSub);
    soma_matriz(b21, b22, s8);      // s8 = b21 + b22
    Matriz *s9 = iniciar_matriz(tamanhoSub, tamanhoSub);
    subtrair_matriz(a11, a21, s9);  // s9 = a11 - a21
    Matriz *s10 = iniciar_matriz(tamanhoSub, tamanhoSub);
    soma_matriz(b11, b12, s10);     // s10 = b11 + b12

    Thread_Args args[7];
    Args_Strassen *tarefas[7];
    tarefas[0] = iniciar_Args_Strassen(a11, s1, limiteTradicional); // p0 = a11 * s1
    tarefas[1] = iniciar_Args_Strassen(s2, b22, limiteTradicional); // p1 = s2 * b22
    tarefas[2] = iniciar_Args_Strassen(s3, b11, limiteTradicional); // p2 = s3 * b11
    tarefas[3] = iniciar_Args_Strassen(a22, s4, limiteTradicional); // p3 = a22 * s4
    tarefas[4] = iniciar_Args_Strassen(s5, s6, limiteTradicional);  // p4 = s5 * s6
    tarefas[5] = iniciar_Args_Strassen(s7, s8, limiteTradicional);  // p5 = s7 * s8
    tarefas[6] = iniciar_Args_Strassen(s9, s10, limiteTradicional); // p6 = s9 * s10

    // Executa a recursão do Algortimo de Strassen de forma concorrente
    for (long i = 0; i < nthreads && i < 7; i++) {
        args[i].id = i;
        args[i].tarefas = tarefas;
        args[i].nthreads = nthreads;
        if(pthread_create(&tids[i], NULL, tarefa_thread, (void *) &args[i])) {
            fprintf(stderr, "ERRO--pthread_create\n");
            exit(3);
        }
    }
    for (long i = 0; i < nthreads && i < 7; i++) {
        pthread_join(tids[i], NULL);
        apagar_Args_Strassen(tarefas[i]);
    }

    // Limpa as matrizes não mais necessárias da memória
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

    soma_matriz(tarefas[3]->ret, tarefas[4]->ret, temp1);     // temp1 = p3 + p4
    subtrair_matriz(tarefas[5]->ret, tarefas[1]->ret, temp2); // temp2 = p5 - p1
    soma_matriz(temp1, temp2, out11);               // out11 = p3 + p4 + p5 - p1
    soma_matriz(tarefas[0]->ret, tarefas[1]->ret, out12);     // out12 = p0 + p1
    soma_matriz(tarefas[2]->ret, tarefas[3]->ret, out21);     // out21 = p2 + p3
    soma_matriz(tarefas[0]->ret, tarefas[4]->ret, temp1);     // temp1 = p0 + p4
    soma_matriz(tarefas[2]->ret, tarefas[6]->ret, temp2);     // temp2 = p2 + p6
    subtrair_matriz(temp1, temp2, out22);           // out22 = p0 + p4 - p2 - p6

    // Limpa as matrizes não mais necessárias da memória
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