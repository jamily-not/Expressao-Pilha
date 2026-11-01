/* Autora: Jamily Notario */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

typedef struct Nodo {
    int prioridade;
    char identificacao[50];
    struct Nodo *prox;
} Nodo;

// TDA da fila
typedef struct Fila {
    Nodo *inicio;
    Nodo *fim;
    int quantidade;
} Fila;

/* Protótipos das funções */
void inicializa_fila(Fila *fila);
Nodo *Cria_Nodo(void);
void insere_nodo_fila(Fila *fila, Nodo *novo);
void insere_fila(Fila *fila, int prioridade, const char *identificacao);
Nodo *remove_fila(Fila *fila);
int carregar_arquivo(Fila *entrada, const char *nomeArquivo);
void exibe_todas_filas(const Fila *entrada, const Fila *maxima, const Fila *normal, const Fila *baixa, Nodo *lote[], int qtdLote);
int distribuir(Fila *entrada, Fila *maxima, Fila *normal, Fila *baixa, Nodo *lote[]);
void libera_fila(Fila *fila);

int main(void) {
    Fila entrada, maxima, normal, baixa;
    const char *nomeArquivo = "impressoraFila.txt";
    int ciclo = 1;

    Nodo *lote[5];
    int qtdLote = 0;

    // Inicialização das 4 filas 
    inicializa_fila(&entrada);
    inicializa_fila(&maxima);
    inicializa_fila(&normal);
    inicializa_fila(&baixa);

    // Carregamento dos dados a partir do arquivo txt
    if (!carregar_arquivo(&entrada, nomeArquivo)) {
        printf("\nNao foi possivel carregar o arquivo '%s'.\n", nomeArquivo);
        printf("Certifique-se de que o arquivo esteja na mesma pasta do programa.\n");
        libera_fila(&entrada);
        return 1;
    }

    printf("\t\tRECEBENDO SOLICITACAO DE IMPRESSAO\n\n");
    exibe_todas_filas(&entrada, &maxima, &normal, &baixa, NULL, 0);

    // Processamento em lotes de até 5 solicitações a cada 0.5 segundo
    while (entrada.inicio != NULL) {
        Sleep(500); // Pausa de 0.5 segundo
        qtdLote = distribuir(&entrada, &maxima, &normal, &baixa, lote);
        printf("\n\t\tEtapa %d: Depois de distribuir ate 5 solicitacoes de impressao\n\n", ciclo++);
        exibe_todas_filas(&entrada, &maxima, &normal, &baixa, lote, qtdLote);
    }

    printf("\n\n\t\tDISTRIBUICAO CONCLUIDA COM SUCESSO DE TODAS AS FILAS!\n");

    // Desalocação final de memória
    libera_fila(&entrada);
    libera_fila(&maxima);
    libera_fila(&normal);
    libera_fila(&baixa);

    return 0;
}

void inicializa_fila(Fila *fila) {
    fila->inicio = NULL;
    fila->fim = NULL;
    fila->quantidade = 0;
}

Nodo *Cria_Nodo(void) {
    Nodo *p;
    p = (Nodo *) malloc(sizeof(Nodo));
    if (!p) {
        printf("Problema de alocacao\n");
        exit(0);
    }
    p->prox = NULL;
    return p;
}

/* Insere um nó no final da fila */
void insere_nodo_fila(Fila *fila, Nodo *novo) {
    novo->prox = NULL;
    if (fila->inicio == NULL) {
        fila->inicio = novo;
    } else {
        fila->fim->prox = novo;
    }
    fila->fim = novo;
    fila->quantidade++;
}

/* Insere um novo elemento no fim da fila (comportamento FIFO) */
void insere_fila(Fila *fila, int prioridade, const char *identificacao) {
    Nodo *novo = Cria_Nodo();
    novo->prioridade = prioridade;
    strncpy(novo->identificacao, identificacao, sizeof(novo->identificacao) - 1);
    novo->identificacao[sizeof(novo->identificacao) - 1] = '\0';
    
    insere_nodo_fila(fila, novo);
}

/* Remove e retorna o nó do início da fila */
Nodo *remove_fila(Fila *fila) {
    Nodo *retirado = fila->inicio;

    if (retirado == NULL) {
        return NULL;
    }

    fila->inicio = retirado->prox;
    if (fila->inicio == NULL) {
        fila->fim = NULL;
    }

    retirado->prox = NULL;
    fila->quantidade--;
    return retirado;
}

/* Lê do arquivo no formato 'prioridade, identificacao;' usando fopen e fscanf */
int carregar_arquivo(Fila *entrada, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    int prioridade;
    char identificacao[50];
    int c;

    if (arquivo == NULL) {
        return 0;
    }

    while (fscanf(arquivo, " %d , %49[^; \t\r\n]", &prioridade, identificacao) == 2) {
        if (prioridade < 1 || prioridade > 3) {
            printf("Prioridade %d invalida na solicitacao %d.\n", prioridade, entrada->quantidade + 1);
            fclose(arquivo);
            return 0;
        }

        insere_fila(entrada, prioridade, identificacao);

        // Avança até o caractere delimitador ';' ou '.'
        while ((c = fgetc(arquivo)) != EOF) {
            if (c == ';' || c == '.') {
                break;
            }
        }
    }

    fclose(arquivo);

    if (entrada->quantidade < 50) {
        printf("\t\tPara registros sera necessario no minimo 50 solicitacoes.\n Encontradas: %d\n", entrada->quantidade);
        return 0;
    }

    return 1;
}

/* Exibe o estado das 4 filas em formato de Tabela com colunas lado a lado e mostra as 5 solicitações lidas no lote */
void exibe_todas_filas(const Fila *entrada, const Fila *maxima, const Fila *normal, const Fila *baixa, Nodo *lote[], int qtdLote) {
    int i;
    printf(" > FILA DE ENTRADA                  [%d itens]:\n\n", entrada->quantidade);
    printf("Identificando IP e prioridade...\n");

    // Mostra as solicitações retiradas no lote atual
    if (qtdLote > 0 && lote != NULL) {
        for (i = 0; i < qtdLote; i++) {
            printf("IP: %-15s \t Prioridade: %d\n", lote[i]->identificacao, lote[i]->prioridade);
        }
    }
    printf("\n");

    // Cabeçalho da Tabela
    printf("+----------------------------------+----------------------------------+----------------------------------+\n");
    printf("| FILA 0 (Prioridade Maxima = 1)   | FILA 1 (Prioridade Normal = 2)   | FILA 2 (Prioridade Baixa = 3)    |\n");
    printf("| Total: %-25d | Total: %-25d | Total: %-25d |\n", maxima->quantidade, normal->quantidade, baixa->quantidade);
    printf("+----------------------------------+----------------------------------+----------------------------------+\n");

    const Nodo *p0 = maxima->inicio;
    const Nodo *p1 = normal->inicio;
    const Nodo *p2 = baixa->inicio;

    if (p0 == NULL && p1 == NULL && p2 == NULL) {
        printf("| %-32s | %-32s | %-32s |\n", "(Vazia)", "(Vazia)", "(Vazia)");
    } else {
        // Percorre as 3 filas simultaneamente linha por linha
        while (p0 != NULL || p1 != NULL || p2 != NULL) {
            char str0[35] = "", str1[35] = "", str2[35] = "";

            if (p0 != NULL) {
                snprintf(str0, sizeof(str0), "[%s | P: %d]", p0->identificacao, p0->prioridade);
                p0 = p0->prox;
            }
            if (p1 != NULL) {
                snprintf(str1, sizeof(str1), "[%s | P: %d]", p1->identificacao, p1->prioridade);
                p1 = p1->prox;
            }
            if (p2 != NULL) {
                snprintf(str2, sizeof(str2), "[%s | P: %d]", p2->identificacao, p2->prioridade);
                p2 = p2->prox;
            }

            printf("| %-32s | %-32s | %-32s |\n", str0, str1, str2);
        }
    }
    printf("+----------------------------------+----------------------------------+----------------------------------+\n");
}

/* Move até 5 elementos da fila de Entrada para as filas de prioridade e salva a referência do lote processado */
int distribuir(Fila *entrada, Fila *maxima, Fila *normal, Fila *baixa, Nodo *lote[]) {
    int i, qtd = 0;
    Nodo *solicitacao;

    for (i = 0; i < 5 && entrada->inicio != NULL; i++) {
        solicitacao = remove_fila(entrada);
        lote[qtd++] = solicitacao;

        if (solicitacao->prioridade == 1) {
            insere_nodo_fila(maxima, solicitacao);
        } else if (solicitacao->prioridade == 2) {
            insere_nodo_fila(normal, solicitacao);
        } else {
            insere_nodo_fila(baixa, solicitacao);
        }
    }
    return qtd;
}

/* Desaloca todos os nós da fila */
void libera_fila(Fila *fila) {
    Nodo *aux;
    while (fila->inicio != NULL) {
        aux = remove_fila(fila);
        free(aux);
    }
}