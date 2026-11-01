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

//TDA da fila
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
void exibe_fila_prioridade(const Fila *fila, const char *titulo);
void exibe_todas_filas(const Fila *entrada, const Fila *maxima, const Fila *normal, const Fila *baixa);
void distribuir(Fila *entrada, Fila *maxima, Fila *normal, Fila *baixa);
void libera_fila(Fila *fila);

int main(void) {
    Fila entrada, maxima, normal, baixa;
    const char *nomeArquivo = "impressoraFila.txt";
    int ciclo = 1;

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

    printf("RECEBENDO SOLICITACAO DE IMPRESSAO\n\n");
    exibe_todas_filas(&entrada, &maxima, &normal, &baixa);

    // Processamento em lotes de até 5 solicitações a cada 0.5 segundo
    while (entrada.inicio != NULL) {
        Sleep(500); // Pausa de 0.5 segundo
        distribuir(&entrada, &maxima, &normal, &baixa);

        printf("\n----------------------------------------------------------------------\n");
        printf("Etapa %d: Depois de distribuir ate 5 solicitacoes de impressao\n\n", ciclo++);
        exibe_todas_filas(&entrada, &maxima, &normal, &baixa);
    }

    printf("\n----------------------------------------------------------------------\n");
    printf("DISTRIBUICAO CONCLUIDA COM SUCESSO DE TODAS AS FILAS!\n");

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

/* insere um novo elemento no fim da fila (comportamento FIFO) */
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
        printf("[AVISO] Para registros sera necessario no minimo 50 solicitacoes.\n Encontradas: %d\n", entrada->quantidade);
        return 0;
    }

    return 1;
}

/* Exibe o conteúdo de cada fila de prioridade na vertical com tabulação (\t) */
void exibe_fila_prioridade(const Fila *fila, const char *titulo) {
    const Nodo *aux = fila->inicio;

    printf("\n%s  - %d itens\n", titulo, fila->quantidade);

    if (aux == NULL) {
        printf("\t(Vazia)\n");
        return;
    }

    while (aux != NULL) {
        printf("\t[%s | Prioridade: %d]\n", aux->identificacao, aux->prioridade);
        aux = aux->prox;
    }
}

/* Exibe o estado das 4 filas */
void exibe_todas_filas(const Fila *entrada, const Fila *maxima, const Fila *normal, const Fila *baixa) {
    printf(" > FILA DE ENTRADA                  [%d itens]:\n\n", entrada->quantidade);
    printf("Identificando IP e prioridade...\n");

    exibe_fila_prioridade(maxima, "FILA 0 (Prioridade Maxima - 1)");
    exibe_fila_prioridade(normal, "FILA 1 (Prioridade Normal - 2)");
    exibe_fila_prioridade(baixa,  "FILA 2 (Prioridade Baixa  - 3)");
}

/* Move até 5 elementos da fila de Entrada para as filas de prioridade */
void distribuir(Fila *entrada, Fila *maxima, Fila *normal, Fila *baixa) {
    int i;
    Nodo *solicitacao;

    for (i = 0; i < 5 && entrada->inicio != NULL; i++) {
        solicitacao = remove_fila(entrada);
        if (solicitacao->prioridade == 1) {
            insere_nodo_fila(maxima, solicitacao);
        } else if (solicitacao->prioridade == 2) {
            insere_nodo_fila(normal, solicitacao);
        } else {
            insere_nodo_fila(baixa, solicitacao);
        }
    }
}

/* Desaloca todos os nós da fila */
void libera_fila(Fila *fila) {
    Nodo *aux;
    while (fila->inicio != NULL) {
        aux = remove_fila(fila);
        free(aux);
    }
}
