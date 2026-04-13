#ifndef LISTA_H
#define LISTA_H

typedef struct registro {
    char sigla[10];
    char municipio[50];

    int julgados;
    int casos_novos;
    int suspensos;
    int dessobrestados;

    int distm2_a, julgm2_a, suspm2_a;
    int distm2_ant, julgm2_ant, suspm2_ant, desom2_ant;
    int distm4_a, julgm4_a, suspm4_a;
    int distm4_b, julgm4_b, suspm4_b;

    struct registro *prox;
} Registro;

typedef struct {
    Registro *inicio;
} Lista;

void inicializar(Lista *l);
void inserir(Lista *l, Registro r);
void liberar(Lista *l);

#endif