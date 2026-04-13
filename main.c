#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "lista.h"

int safe_atoi(char *s) {
    if (!s || strlen(s) == 0) return 0;
    return atoi(s);
}

void remove_aspas(char *s) {
    if (!s) return;

    int len = strlen(s);

    if (len > 0 && s[0] == '"') {
        memmove(s, s + 1, len);
        len--;
    }

    if (len > 0 && s[len - 1] == '"') {
        s[len - 1] = '\0';
    }
}

void ler_csv(const char *nome, Lista *lista) {
    FILE *arq = fopen(nome, "r");
    if (!arq) return;

    char linha[2048];
    fgets(linha, sizeof(linha), arq);

    while (fgets(linha, sizeof(linha), arq)) {
        Registro r = {0};

        char *tok = strtok(linha, ",");
        int col = 1;

        while (tok) {
            remove_aspas(tok);

            switch (col) {
                case 1: strcpy(r.sigla, tok); break;
                case 6: strcpy(r.municipio, tok); break;

                case 11: r.casos_novos = safe_atoi(tok); break;
                case 12: r.julgados = safe_atoi(tok); break;
                case 14: r.suspensos = safe_atoi(tok); break;
                case 15: r.dessobrestados = safe_atoi(tok); break;

                case 17: r.distm2_a = safe_atoi(tok); break;
                case 18: r.julgm2_a = safe_atoi(tok); break;
                case 19: r.suspm2_a = safe_atoi(tok); break;

                case 21: r.distm2_ant = safe_atoi(tok); break;
                case 22: r.julgm2_ant = safe_atoi(tok); break;
                case 23: r.suspm2_ant = safe_atoi(tok); break;
                case 24: r.desom2_ant = safe_atoi(tok); break;

                case 26: r.distm4_a = safe_atoi(tok); break;
                case 27: r.julgm4_a = safe_atoi(tok); break;
                case 28: r.suspm4_a = safe_atoi(tok); break;

                case 30: r.distm4_b = safe_atoi(tok); break;
                case 31: r.julgm4_b = safe_atoi(tok); break;
                case 32: r.suspm4_b = safe_atoi(tok); break;
            }

            tok = strtok(NULL, ",");
            col++;
        }

        inserir(lista, r);
    }

    fclose(arq);
}

void carregar_todos(Lista *lista) {
    DIR *dir = opendir(".");
    if (!dir) return;

    struct dirent *e;

    while ((e = readdir(dir)) != NULL) {
        if (strstr(e->d_name, ".csv") &&
            strcmp(e->d_name, "concatenado.csv") != 0 &&
            strcmp(e->d_name, "resumo.csv") != 0) {

            printf("lendo %s\n", e->d_name);
            ler_csv(e->d_name, lista);
        }
    }

    closedir(dir);
}

void concatenar() {
    FILE *out = fopen("concatenado.csv", "w");
    if (!out) return;

    DIR *dir = opendir(".");
    if (!dir) return;

    struct dirent *e;
    char linha[2048];
    int primeiro = 1;

    while ((e = readdir(dir)) != NULL) {

        // só pega os csv originais
        if (strstr(e->d_name, "teste_") && strstr(e->d_name, ".csv")) {

            FILE *arq = fopen(e->d_name, "r");
            if (!arq) continue;

            int cab = 1;

            while (fgets(linha, sizeof(linha), arq)) {

                // evita repetir cabeçalho
                if (!primeiro && cab) {
                    cab = 0;
                    continue;
                }

                fputs(linha, out);
                cab = 0;
            }

            primeiro = 0;
            fclose(arq);
        }
    }

    closedir(dir);
    fclose(out);

    printf("concatenado pronto\n");
}

void filtrar(Lista *lista, char *nome) {
    char arqnome[100];
    sprintf(arqnome, "%s.csv", nome);

    FILE *out = fopen(arqnome, "w");
    if (!out) return;

    Registro *atual = lista->inicio;

    while (atual) {
        if (strcmp(atual->municipio, nome) == 0) {
            fprintf(out, "%s,%s,%d\n",
                atual->sigla,
                atual->municipio,
                atual->julgados);
        }

        atual = atual->prox;
    }

    fclose(out);
    printf("arquivo gerado\n");
}

void gerar_resumo(Lista *lista) {
    FILE *out = fopen("resumo.csv", "w");
    if (!out) return;

    fprintf(out, "sigla,total_julgados,meta1,meta2a,meta2ant,meta4a,meta4b\n");

    Registro *i = lista->inicio;

    while (i) {
        char sigla[10];
        strcpy(sigla, i->sigla);

        int soma_julgados = 0, soma_casos = 0, soma_susp = 0, soma_desso = 0;
        int d2a=0,j2a=0,s2a=0;
        int d2ant=0,j2ant=0,s2ant=0,des2ant=0;
        int d4a=0,j4a=0,s4a=0;
        int d4b=0,j4b=0,s4b=0;

        Registro *j = lista->inicio;

        while (j) {
            if (strcmp(j->sigla, sigla) == 0) {
                soma_julgados += j->julgados;
                soma_casos += j->casos_novos;
                soma_susp += j->suspensos;
                soma_desso += j->dessobrestados;

                d2a += j->distm2_a;
                j2a += j->julgm2_a;
                s2a += j->suspm2_a;

                d2ant += j->distm2_ant;
                j2ant += j->julgm2_ant;
                s2ant += j->suspm2_ant;
                des2ant += j->desom2_ant;

                d4a += j->distm4_a;
                j4a += j->julgm4_a;
                s4a += j->suspm4_a;

                d4b += j->distm4_b;
                j4b += j->julgm4_b;
                s4b += j->suspm4_b;
            }
            j = j->prox;
        }

        float meta1 = 0, meta2a = 0, meta2ant = 0, meta4a = 0, meta4b = 0;

        int den1 = soma_casos + soma_desso - soma_susp;
        if (den1) meta1 = (float)soma_julgados / den1 * 100;

        int den2a = d2a - s2a;
        if (den2a) meta2a = (float)j2a / den2a * (1000.0/7.0);

        int den2ant = d2ant - s2ant - des2ant;
        if (den2ant) meta2ant = (float)j2ant / den2ant * 100;

        int den4a = d4a - s4a;
        if (den4a) meta4a = (float)j4a / den4a * 100;

        int den4b = d4b - s4b;
        if (den4b) meta4b = (float)j4b / den4b * 100;

        fprintf(out, "%s,%d,%.2f,%.2f,%.2f,%.2f,%.2f\n",
            sigla, soma_julgados,
            meta1, meta2a, meta2ant, meta4a, meta4b
        );

        i = i->prox;
        while (i && strcmp(i->sigla, sigla) == 0)
            i = i->prox;
    }

    fclose(out);
    printf("resumo gerado\n");
}

int main() {
    Lista lista;
    inicializar(&lista);

    int op;
    char municipio[50];

    do {
        printf("\n1 concatenar\n2 carregar\n3 filtrar\n4 resumo\n0 sair\n> ");
        scanf("%d", &op);

        switch (op) {
            case 1: concatenar(); break;
            case 2: carregar_todos(&lista); printf("carregado\n"); break;
            case 3:
                printf("municipio: ");
                scanf("%s", municipio);
                filtrar(&lista, municipio);
                break;
            case 4: gerar_resumo(&lista); break;
        }

    } while (op != 0);

    liberar(&lista);
    return 0;
}