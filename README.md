# Compreensão sobre Pilha e Fila

Este repositório armazena dois programas desenvolvidos para a disciplina de Estrutura de Dados I. Seu principal objetivo é a compreensão e a capacidade de aplicar as estruturas de dados **Pilha** e **Fila** na resolução de problemas práticos.

---

# Sobre `pilha_expressoes.c`

O programa converte expressões infixas para pósfixas e préfixas, realiza validações sintáticas e calcula o resultado numérico das expressões utilizando a estrutura de **Pilha (LIFO)**.

## O que é uma Expressão Pósfixa e Préfixa
- **Pósfixa (Notação Polonesa Reversa):** Os operadores são posicionados após os operandos. A resolução ocorre da esquerda para a direita ($\rightarrow$).
- **Préfixa (Notação Polonesa):** Os operadores são posicionados antes dos operandos. A resolução ocorre da direita para a esquerda ($\leftarrow$).

### Exemplo: `2 + 3 * 4`
- [ ] **Pósfixa:** `2 3 4 * +` $\rightarrow$
- [x] **Préfixa:** `+ * 4 3 2` $\leftarrow$

---

## Algoritmo Shunting Yard

A conversão para a notação pósfixa baseia-se no algoritmo **Shunting Yard** (desenvolvido por Edsger Dijkstra). Ele armazena temporariamente os operadores em uma pilha durante a leitura da expressão e os desempilha conforme a regra de precedência matemática.

Este algoritmo resolve a prioridade de operadores e o tratamento de parênteses em expressões matemáticas. Enquanto para humanos a expressão `2 + 3 * 4` é intuitiva, para o computador a notação pósfixa elimina a necessidade de parênteses e simplifica o cálculo.

### Funcionamento Análogo ao Pátio de Manobras de Trens

A conversão funciona de forma análoga a um **pátio de manobras de trens**: os vagões (números e operadores) são direcionados para trilhos de desvio (representados pelas **pilhas**).

Ao identificar uma expressão como `2 * 3 + 4`:
1. Valida-se se o elemento lido é um número ou operador.
2. Se for **número**, envia-se diretamente para a **saída**.
3. Se for **operador**, armazena-se na **pilha**.

**Passo a passo da simulação (`2 * 3 + 4`):**
- Leitura do `2` $\rightarrow$ Envia para a **saída** (`2`).
- Leitura do `*` $\rightarrow$ Empilha na **pilha** (`*`).
- Leitura do `3` $\rightarrow$ Envia para a **saída** (`2 3`).
- Leitura do operador `+` $\rightarrow$ Compara precedência com o topo (`*`). Como `*` tem maior prioridade, **desempilha-se** o `*` para a saída (`2 3 *`) e **empilha-se** o `+`.
- Leitura do `4` $\rightarrow$ Envia para a **saída** (`2 3 * 4`).
- Fim da expressão (`\0`) $\rightarrow$ **Desempilha-se** o restante da pilha (`+`) para a saída.
- **Resultado final pósfixo:** `2 3 * 4 +`.

---

## Fluxo da `main` e Arquitetura do Programa (`pilha_expressoes.c`)

A função `main` gerencia a execução, inicializa as listas (`expressao` e `resultados`), captura as entradas e apresenta o menu de opções: **1. Pósfixa**, **2. Préfixa**, **3. Validação**, **4. Cálculo** e **5. Sair**.

```text
main()
│
├── inicializaLista(&expressao, &resultados)
│
├── enquanto (opcao != SAIR):
│    ├── entraValores(&expressao)
│    │    └── adicionaExpressao()  -->  reservaMemoria() / malloc()
│    │
│    ├── [1] Pósfixa: infixoParaPostfixa()
│    │    ├── ehOp() / ehUnario() / precedencia()
│    │    ├── empilharOp() / desempilharOp()
│    │    └── escreveOp()
│    │
│    ├── [2] Préfixa: convertePosfixaParaPrefixa()
│    │    └── empilhaTEXTO() / desempilhaTEXTO()
│    │
│    ├── [3] Validação: valida()
│    │    └── identificaOp()
│    │
│    └── [4] Cálculo: calculo()
│         ├── empilhaNum() / desempilharNum()
│         └── liberaNum()
│
└── liberaExpressao(&expressao)  -->  free()
```

---

# Sobre `fila_impressora.c`

O programa gerencia um sistema de fila de impressão utilizando a estrutura de **Fila Encadeada com Prioridade (FIFO - *First In, First Out*)**.

## Como Funciona o Programa?

O programa lê as solicitações de impressão contendo o **IP** e a **PRIORIDADE** a partir de um arquivo `.txt` (`impressoraFila.txt`).

O sistema utiliza **4 filas encadeadas**:
1. **Fila de Entrada:** Armazena todas as solicitações lidas inicialmente do arquivo TXT.
2. **Filas de Prioridade:**
   - **Fila 0:** Prioridade Máxima (`1`)
   - **Fila 1:** Prioridade Normal (`2`)
   - **Fila 2:** Prioridade Baixa (`3`)

As solicitações são processadas em lotes de até **5 itens por ciclo** (com intervalo de 0,5s configurado via `Sleep`), sendo removidas do **início** da Fila de Entrada e engatadas no **fim** da Fila de Prioridade correspondente.

---

## Estrutura e Etapas do Programa (`fila_impressora.c`)

- **2 TDAs (Tipos Abstratos de Dados):**
  - `Nodo`: Nó da estrutura que armazena a `prioridade`, a identificação (`IP`) e o ponteiro `prox`.
  - `Fila`: Descritor da fila contendo os ponteiros `inicio`, `fim` e o contador `quantidade`.

### Mapa de Execução e Chamadas de Funções

```text
main()
│
├── inicializa_fila(&entrada, &maxima, &normal, &baixa)
│
├── carregar_arquivo(&entrada, "impressoraFila.txt")
│    ├── fopen() / fscanf() / fclose()
│    └── insere_fila()
│         ├── Cria_Nodo()  -->  malloc()
│         └── insere_nodo_fila()
│
├── exibe_todas_filas()  [Exibição Inicial]
│
└── enquanto (entrada.inicio != NULL):
     ├── Sleep(500)
     ├── distribuir(&entrada, &maxima, &normal, &baixa, lote)
     │    ├── remove_fila(&entrada)
     │    └── insere_nodo_fila(&maxima | &normal | &baixa)
     │
     ├── exibe_todas_filas()  [Exibe lote lido + Tabela atualizada]
     │
     └── libera_fila(&entrada, &maxima, &normal, &baixa)
          ├── remove_fila()
          └── free()
```