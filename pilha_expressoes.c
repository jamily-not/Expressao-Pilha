/* Autora: Jamily Notario */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#define MAX_EXPRESSAO 300

typedef struct PilhaOperadores {
    char operador;
    int unario; // Distingue subtracao de sinal negativo
    struct PilhaOperadores *prox;
} PilhaOperadores;

typedef struct PilhaNumeros {
    double numero;
    struct PilhaNumeros *prox;
} PilhaNumeros;

typedef struct PilhaTextos { // auxiliar para a prefixa
    char *texto;
    struct PilhaTextos *prox;
} PilhaTextos;

typedef struct Expressao {
    char *texto; // Endereco de um texto terminado por '\0'.
    struct Expressao *prox;
} Expressao;

void *reservarMemoria(int tamanho);

Expressao *cria_expressao(const char *texto);
void inicializaLista(Expressao **lista);
int listaVazia(Expressao **p);
void adiciona_expressao(Expressao **lista, Expressao *nova);
void liberarExpressoes(Expressao **lista);
void entraValores(Expressao **lista);

// A regra e sempre LIFO, mas cada par tem parametros do seu tipo.
void empilharOperador(PilhaOperadores **topo, char operador, int unario);
int desempilharOperador(PilhaOperadores **topo, char *operador, int *unario);

void empilharNumero(PilhaNumeros **topo, double numero);
int desempilharNumero(PilhaNumeros **topo, double *numero);

void liberarNumeros(PilhaNumeros **topo);

void empilharTexto(PilhaTextos **topo, const char *texto);
char *desempilharTexto(PilhaTextos **topo);

int precedencia(char operador, int unario);
int ehSinalUnario(const char *texto);
int ehOperador(const char *texto);
int identificarOperador(char operador, int esperaOperando, int *unario);
char *infixaParaPosfixa(const char *entrada); // Shunting Yard de uma expressao valida.
void converterPosfixa(Expressao *lista, Expressao **resultados); // Aplica a conversao a cada entrada da lista.
void converterPrefixa(Expressao *lista, Expressao **resultados);
int validarExpressao(const char *entrada);
void calcularExpressao(Expressao *lista, Expressao **resultados);
void imprimirResultados(Expressao *lista, Expressao *resultados);

int main(void) {
    Expressao *lista;
    Expressao *resultados;
    Expressao *aux; // Usado somente para mostrar a validacao na opcao 3.
    int opcao = 0, leitura, caractere;

    inicializaLista(&lista);
    inicializaLista(&resultados);
    entraValores(&lista);
    if (listaVazia(&lista)) return 0;

    do {
    	system("pause");
    	system("cls");
        printf("\n       EXPRESSOES ARITMETICAS     \n");
        printf("\t1) Infixa para posfixa\n");
        printf("\t2) Infixa para prefixa\n");
        printf("\t3) Validar expressoes\n");
        printf("\t4) Calcular expressoes\n");
        printf("\t5) Sair\nOpcao: ");
        leitura = scanf("%d", &opcao);
        
        if (leitura == EOF) opcao = 5;
        else {
            if (leitura != 1) opcao = 0;
            while ((caractere = getchar()) != '\n' && caractere != EOF) {}
        }

        switch (opcao) {
            case 1:
                converterPosfixa(lista, &resultados);
                break;
            case 2:
                converterPrefixa(lista, &resultados);
                break;
            case 3:
                for (aux = lista; aux != NULL; aux = aux->prox) {
                    if (validarExpressao(aux->texto)) {
                        printf("%s = Expressao valida\n", aux->texto);
                    } else {
                        printf("%s = Expressao invalida\n", aux->texto);
                    }
                }
                break;
            case 4:
                calcularExpressao(lista, &resultados);
                break;
            case 5:
                liberarExpressoes(&lista);
                printf("Liberando lista de expressoes...\n");
                printf("Encerrando o programa.\n");
                break;
            default:
                printf("Opcao invalida.\n");
        }

        if (!listaVazia(&resultados)) {
            imprimirResultados(lista, resultados);
            printf("Liberando lista de resultados...\n");
            liberarExpressoes(&resultados);
        }
    } while (opcao != 5);
    return 0;
}

void *reservarMemoria(int tamanho) {
    void *memoria = malloc(tamanho);
    if (memoria == NULL) {
        printf("Problema de alocacao\n");
        exit(1);
    }
    return memoria;
}

Expressao *cria_expressao(const char *texto) {
    Expressao *nova = (Expressao *) reservarMemoria(sizeof(Expressao));
    nova->texto = (char *) reservarMemoria(strlen(texto) + 1);
    strcpy(nova->texto, texto);
    nova->prox = NULL;
    return nova;
}

void inicializaLista(Expressao **lista) {
    *lista = NULL;
}

int listaVazia(Expressao **p) {
    return *p == NULL;
}

// Insere no fim para preservar a ordem em que o usuario digitou.
void adiciona_expressao(Expressao **lista, Expressao *nova) {
    Expressao *aux;
    if (*lista == NULL) *lista = nova;
    else {
        aux = *lista;
        while (aux->prox != NULL) aux = aux->prox;
        aux->prox = nova;
    }
}

void liberarExpressoes(Expressao **lista) {
    Expressao *remover;
    while (*lista != NULL) {
        remover = *lista;
        *lista = remover->prox;
        free(remover->texto);
        free(remover);
    }
}

void entraValores(Expressao **lista) {
    char entrada[MAX_EXPRESSAO + 1];
    char resposta[4];
    int continuar = 1, caractere;

    while (continuar) {
        printf("Digite a Expressao (ate %d caracteres; numeros reais com ponto): ", MAX_EXPRESSAO);
        if (fgets(entrada, sizeof(entrada), stdin) == NULL) return;

        if (strchr(entrada, '\n') == NULL) {
            caractere = getchar();
            if (caractere != '\n' && caractere != EOF) {
                while ((caractere = getchar()) != '\n' && caractere != EOF) {}
                printf("Limite de %d caracteres excedido. Digite novamente.\n", MAX_EXPRESSAO);
                continue;
            }
        }
        entrada[strcspn(entrada, "\r\n")] = '\0'; // Retira o Enter.

        if (entrada[0] != '\0') {
            adiciona_expressao(lista, cria_expressao(entrada));
        } else {
            printf("Linha vazia: nenhuma expressao adicionada.\n");
        }

        do {
            printf("Inserir outra? 1 = sim, 0 = nao: ");
            if (fgets(resposta, sizeof(resposta), stdin) == NULL) return;
            continuar = -1;
            // Se a resposta for longa demais, descarta o restante da linha.
            if (strchr(resposta, '\n') == NULL && !feof(stdin)) {
                while ((caractere = getchar()) != '\n' && caractere != EOF) {}
            } else {
                resposta[strcspn(resposta, "\r\n")] = '\0';
                if (strcmp(resposta, "1") == 0) continuar = 1;
                else if (strcmp(resposta, "0") == 0) continuar = 0;
            }
            if (continuar == -1) printf("Digite 1 ou 0.\n");
        } while (continuar == -1);
    }
}

// operacao sobre a pilha de operadores
void empilharOperador(PilhaOperadores **topo, char operador, int unario) {
    PilhaOperadores *novo = (PilhaOperadores *) reservarMemoria(sizeof(PilhaOperadores));
    novo->operador = operador;
    novo->unario = unario;
    novo->prox = *topo;
    *topo = novo;
}

int desempilharOperador(PilhaOperadores **topo, char *operador, int *unario) {
    PilhaOperadores *retirado = *topo;
    if (retirado == NULL) return 0;
    *operador = retirado->operador;
    *unario = retirado->unario;
    *topo = retirado->prox;
    free(retirado);
    return 1;
}

//operacao sobnre a pilha de numeros
void empilharNumero(PilhaNumeros **topo, double numero) {
    PilhaNumeros *novo = (PilhaNumeros *) reservarMemoria(sizeof(PilhaNumeros));
    novo->numero = numero;
    novo->prox = *topo;
    *topo = novo;
}

int desempilharNumero(PilhaNumeros **topo, double *numero) {
    PilhaNumeros *retirado = *topo;
    if (retirado == NULL) return 0;
    *numero = retirado->numero;
    *topo = retirado->prox;
    free(retirado);
    return 1;
}

void liberarNumeros(PilhaNumeros **topo) {
    double descartado;
    while (desempilharNumero(topo, &descartado)) {
    }
}

// operacao sobre a pilha de textos que a prefixa utiliza
void empilharTexto(PilhaTextos **topo, const char *texto) {
    PilhaTextos *novo = (PilhaTextos *) reservarMemoria(sizeof(PilhaTextos));
    novo->texto = (char *) reservarMemoria(strlen(texto) + 1);
    strcpy(novo->texto, texto);
    novo->prox = *topo;
    *topo = novo;
}

char *desempilharTexto(PilhaTextos **topo) {
    PilhaTextos *retirado = *topo;
    char *texto;
    if (retirado == NULL) return NULL;
    texto = retirado->texto;
    *topo = retirado->prox;
    free(retirado);
    return texto;
}

int precedencia(char operador, int unario) {
    if (unario) return 3;

    switch (operador) {
        case '+':
        case '-': return 1;
        case '*':
        case '/': return 2;
        case '^': return 4;
        default:  return 0; // Nao e um operador.
    }
}

//1 se for "(-)" ou "(+)", 0 caso contrario.
int ehSinalUnario(const char *texto) {
    return strcmp(texto, "(-)") == 0 || strcmp(texto, "(+)") == 0;
}
//1 se for um sinal unario "(-)"/"(+)" ou operador com precedencia > 0.
int ehOperador(const char *texto) {
    return ehSinalUnario(texto) || (strlen(texto) == 1 && precedencia(texto[0], 0) > 0);
}

//faz validacao dos operadores unarios
int identificarOperador(char operador, int esperaOperando, int *unario) {
    *unario = 0;
    if (precedencia(operador, 0) == 0) return 0;
    if (esperaOperando) {
        if (operador != '-' && operador != '+') return 0;
        *unario = 1;
    }
    return 1;
}

// identifica o operador unario com ()
void escreverOperador(char *saida, int *posicao, char operador, int unario) {
    if (unario) {
        saida[*posicao] = '(';
        (*posicao)++;
    }
    saida[*posicao] = operador;
    (*posicao)++;
    if (unario) {
        saida[*posicao] = ')';
        (*posicao)++;
    }
    saida[*posicao] = ' ';
    (*posicao)++;
}


/* Checa se os parenteses estao balanceados, 
se a pontuacao decimal  nos numeros reais esta correta
se a sequencia de operadores/operandos obedece a logica da matematica. */
int validarExpressao(const char *entrada) {
    int i = 0;
    int esperaOperando = 1;
    int parenteses = 0; // Quantos '(' ainda precisam de um ')'.
    int digitos, pontos;
    int unario;
    char atual;

    while (entrada[i] != '\0') {
        atual = entrada[i];

        if (isspace((unsigned char)atual)) {
            i++;
        } else if (isdigit((unsigned char)atual) || atual == '.') {
            if (!esperaOperando) return 0; 
            digitos = 0;
            pontos = 0;
            while (isdigit((unsigned char)entrada[i]) || entrada[i] == '.') {
                if (entrada[i] == '.') pontos++;
                else digitos++;
                i++;
            }
            if (digitos == 0 || pontos > 1) return 0;
            esperaOperando = 0;
        } else if (atual == '(') {
            if (!esperaOperando) return 0; // Nao aceitamos 2(3+4).
            parenteses++;
            i++;
        } else if (atual == ')') {
            if (esperaOperando || parenteses == 0) return 0;
            parenteses--;
            i++;
        } else {
            // Aceita somente operadores conhecidos, na posicao correta.
            // Se falta um numero, apenas + e - podem ser sinais.
            if (!identificarOperador(atual, esperaOperando, &unario)) return 0;
            esperaOperando = 1;
            i++;
        }
    }

    // Impede entrada vazia, operador sem numero e parenteses sem fechamento.
    if (esperaOperando || parenteses != 0) return 0;
    return 1;
}

/* ALGORITMO DE SHUNTING YARD
   recebe UMA expressao que ja passou por validarExpressao.
   Retorna o texto convertido */
char *infixaParaPosfixa(const char *entrada) {
    PilhaOperadores *pilhaOp = NULL;
    char *saida;
    char atual, operador;
    int i = 0;
    int j = 0; 
    int tamanho = strlen(entrada);
    int esperaOperando = 1; // Aqui serve apenas para reconhecer sinais unarios.
    int unario;
    int prioridadeTopo, prioridadeAtual;

    // Um sinal pode virar "(-) ": reservamos ate quatro vezes o tamanho.
    saida = (char *) reservarMemoria(tamanho * 4 + 1);

    while (entrada[i] != '\0') {
        atual = entrada[i];

        // Ignora espacos
        if (isspace((unsigned char)atual)) {
            i++;
        }
        // Copia o numero completo, incluindo suas casas decimais.
        else if (isdigit((unsigned char)atual) || atual == '.') {
            while (isdigit((unsigned char)entrada[i]) || entrada[i] == '.') {
                saida[j] = entrada[i];
                j++;
                i++;
            }
            saida[j] = ' ';
            j++;
            esperaOperando = 0;
        }
        // Marca na pilhaOp o inicio de um grupo.
        else if (atual == '(') {
            empilharOperador(&pilhaOp, '(', 0);
            esperaOperando = 1;
            i++;
        }
        // Retira os operadores do grupo e descarta o '(' - validaExpressao garante que tem ')'
        else if (atual == ')') {
            while (pilhaOp != NULL && pilhaOp->operador != '(') {
                desempilharOperador(&pilhaOp, &operador, &unario);
                escreverOperador(saida, &j, operador, unario);
            }
            desempilharOperador(&pilhaOp, &operador, &unario);
            esperaOperando = 0;
            i++;
        }
        // Guarda sinais ou compara a prioridade das operacoes.
        else {
            if (esperaOperando) {
                // Empilha o sinal sem retirar os operadores anteriores.
                empilharOperador(&pilhaOp, atual, 1);
            } else {
                prioridadeAtual = precedencia(atual, 0);
                while (pilhaOp != NULL && pilhaOp->operador != '(') {
                    prioridadeTopo = precedencia(pilhaOp->operador, pilhaOp->unario);
                    if (prioridadeTopo < prioridadeAtual) break;
                    // 2^3^2 significa 2^(3^2): potencias iguais esperam.
                    if (prioridadeTopo == prioridadeAtual && atual == '^') break;
                    desempilharOperador(&pilhaOp, &operador, &unario);
                    escreverOperador(saida, &j, operador, unario);
                }
                empilharOperador(&pilhaOp, atual, 0);
                esperaOperando = 1;
            }
            i++;
        }
    }

    // envia os operadores restantes para a saida.
    // a pilhaOp termina vazia.
    while (pilhaOp != NULL) {
        desempilharOperador(&pilhaOp, &operador, &unario);
        escreverOperador(saida, &j, operador, unario);
    }
    saida[j - 1] = '\0'; // Troca o ultimo espaco pelo fim do texto.
    return saida;
}

// Percorre a lista de expressao e chama a conversao para cada nodo da lista
void converterPosfixa(Expressao *lista, Expressao **resultados) {
    Expressao *aux;
    char *texto;
    for (aux = lista; aux != NULL; aux = aux->prox) {
        if (!validarExpressao(aux->texto)) {
            adiciona_expressao(resultados, cria_expressao("Expressao invalida"));
        } else {
            texto = infixaParaPosfixa(aux->texto);
            adiciona_expressao(resultados, cria_expressao(texto));
            free(texto);
        }
    }
}

/*
Primeiro converte a expressao para posfixa. 
Em seguida, percorre as partes da expressao e usa uma pilha para reordenar
*/
void converterPrefixa(Expressao *lista, Expressao **resultados) {
    Expressao *aux;
    PilhaTextos *pilha;
    char *posfixa, *ptExpressao, *esquerda, *direita, *combinado, *texto;
    int unario, operador;

    for (aux = lista; aux != NULL; aux = aux->prox) {
        if (!validarExpressao(aux->texto)) {
            adiciona_expressao(resultados, cria_expressao("Expressao invalida"));
            continue;
        }
        posfixa = infixaParaPosfixa(aux->texto);
        pilha = NULL;
        // strtok separa a posfixa pelos espacos.
        // ptExpressao aponta para uma parte por vez: numero ou operador.
        ptExpressao = strtok(posfixa, " ");
        while (ptExpressao != NULL) {
            unario = ehSinalUnario(ptExpressao);
            operador = ehOperador(ptExpressao);
            if (!operador) empilharTexto(&pilha, ptExpressao);
            else {
                direita = desempilharTexto(&pilha);
                if (unario) esquerda = NULL;
                else esquerda = desempilharTexto(&pilha);
                combinado = (char *) reservarMemoria(strlen(ptExpressao) + strlen(direita) +
                             (esquerda != NULL ? strlen(esquerda) : 0) + 3);
                if (unario) sprintf(combinado, "%s %s", ptExpressao, direita);
                else sprintf(combinado, "%s %s %s", ptExpressao, esquerda, direita);
                empilharTexto(&pilha, combinado);
                free(esquerda);
                free(direita);
                free(combinado);
            }
            ptExpressao = strtok(NULL, " ");
        }
        texto = desempilharTexto(&pilha);
        adiciona_expressao(resultados, cria_expressao(texto));
        free(texto);
        free(posfixa);
    }
}

/*
Converte a expressao para notacao posfixa e processa parte por parte da expressao com  auxilio de uma pilha de numeros 
Ao encontrar numeros, empilha, ao encontrar operadores, desempilha a quantidade necessaria de valores
calcula a operacao (trata divisao por zero) e empilha o resultado parcial.
*/
void calcularExpressao(Expressao *lista, Expressao **resultados) {
    Expressao *aux;
    PilhaNumeros *pilha;
    char *posfixa, *ptExpressao, *fim;
    char texto[64];
    double esquerda, direita, valor;
    int valido, unario, operador;

    for (aux = lista; aux != NULL; aux = aux->prox) {
        if (!validarExpressao(aux->texto)) {
            adiciona_expressao(resultados, cria_expressao("Expressao invalida"));
            continue;
        }
        posfixa = infixaParaPosfixa(aux->texto);
        pilha = NULL;
        valido = 1;
        valor = 0;
        // strtok separa a posfixa pelos espacos.
        // ptExpressao aponta para uma parte por vez: numero ou operador.
        ptExpressao = strtok(posfixa, " ");
        while (ptExpressao != NULL && valido) {
            unario = ehSinalUnario(ptExpressao);
            operador = ehOperador(ptExpressao);
            if (!operador) {
                valor = strtod(ptExpressao, &fim);
                if (*fim != '\0') valido = 0;
            } else {
                valido = desempilharNumero(&pilha, &direita);
                if (valido && unario) valor = ptExpressao[1] == '-' ? -direita : direita;
                else if (valido) {
                    valido = desempilharNumero(&pilha, &esquerda);
                    if (valido) {
                        switch (ptExpressao[0]) {
                            case '+': valor = esquerda + direita; break;
                            case '-': valor = esquerda - direita; break;
                            case '*': valor = esquerda * direita; break;
                            case '/':
                                if (direita == 0) valido = 0;
                                else valor = esquerda / direita;
                                break;
                            case '^': valor = pow(esquerda, direita); break;
                        }
                    }
                }
            }
            if (valido) empilharNumero(&pilha, valor);
            ptExpressao = strtok(NULL, " ");
        }
        if (valido) {
            valido = desempilharNumero(&pilha, &valor);
            if (pilha != NULL) valido = 0;
        }
        if (valido) {
            if (floor(valor) == valor) {
    			snprintf(texto, sizeof(texto), "%.0f", valor); // Ex: imprime 100 em vez de 100.0000
			} else {
    			snprintf(texto, sizeof(texto), "%.6g", valor);  // Ex: imprime 12.5 para decimais
			}
            adiciona_expressao(resultados, cria_expressao(texto));
        } else {
            adiciona_expressao(resultados, cria_expressao("Erro de calculo (divisao por zero)"));
        }
        liberarNumeros(&pilha);
        free(posfixa);
    }
}

void imprimirResultados(Expressao *lista, Expressao *resultados) {
    while (lista != NULL && resultados != NULL) {
        printf("%s = %s\n", lista->texto, resultados->texto);
        lista = lista->prox;
        resultados = resultados->prox;
    }
}