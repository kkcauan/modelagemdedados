#include <stdio.h>
#include <stdlib.h>
#include "lista.h"

void inicializar(Lista *l) {
    l->inicio = NULL;
}

void inserir(Lista *l, Registro r) {
    Registro *novo = malloc(sizeof(Registro));
    if (!novo) return;

    *novo = r;
    novo->prox = l->inicio;
    l->inicio = novo;
}

void liberar(Lista *l) {
    Registro *atual = l->inicio;

    while (atual) {
        Registro *tmp = atual;
        atual = atual->prox;
        free(tmp);
    }

    l->inicio = NULL;
}
