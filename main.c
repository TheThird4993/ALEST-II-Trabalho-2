#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define false 0;
#define true 1;

typedef struct nodo nodo;
typedef struct queueNode queueNode;
typedef struct queue queue;

struct nodo {
    nodo* norte, * sul, * leste, * oeste;
    char tipo;      // 'p' para porto, 'c' para caminho, 'i' para intransitável
    int porto;      // número do porto, ou 0
    int num;        // índice linear na matriz
};

struct queueNode {
    nodo* nodo;
    queueNode* next;
};

struct queue {
    queueNode* front;
    queueNode* rear;
};

struct queueNode* createqueueNode(nodo*);
void enqueueNode(queue*, nodo*);
int dequeueNode(queue*);
char typecheck(queue*);
struct queue* createQueue();
void clearQueue(queue*);
nodo** funcMatriz(int, int);
void freeMatriz(int, int, nodo**);
void printMatriz(int, int, nodo**);
nodo* insereMatriz(int, int, nodo**, FILE*);
void BFS(queue*, nodo**, int, _Bool*, nodo*, int, int);

int main() {
    int lin, col;
    nodo** matriz;
    FILE* file;
    clock_t inicio, fim;
    double tempo_gasto;
    char destino[32] = "casos/case";
    int num;

    do{
        printf("escolha o caso 0 a 5: ");
        scanf("%d", &num);
    }while(num < 0 || num > 5);
    
    inicio = clock();

    size_t len = strlen(destino);
    destino[len] = num + '0';
    destino[len + 1] = '\0';

    strcat(destino, ".map");

    file = fopen(destino, "r");
    if (!file) {
        perror("Erro ao abrir arquivo");
        return 1;
    }

    /*
    //-------------------------------------------  para imprimir o arquivo
    int c;
    while ((c = fgetc(file)) != EOF) {
        printf("%c", c);
    }
    printf("\n");
    rewind(file);                               // para retornar o ponteiro de leitura ao inicio do arquivo
    //-------------------------------------------
    */

    fscanf(file, "%d %d", &lin, &col);

    matriz = funcMatriz(lin, col);

    printf("linhas = %d\ncolunas = %d\n", lin, col);
    nodo* nodoInicial = insereMatriz(lin, col, matriz, file);
    //-------------------------------------------

    int tamanho = lin * col;
    _Bool* jaVisitado = (_Bool*)calloc(tamanho, sizeof(_Bool));
    queue* fila = createQueue();
    enqueueNode(fila, nodoInicial);

    BFS(fila, matriz, nodoInicial->porto, jaVisitado, nodoInicial, lin, col);

    //printMatriz(lin, col, matriz);
    fim = clock(); // Marca o fim
    tempo_gasto = ((double)(fim - inicio)) / CLOCKS_PER_SEC;

    printf("Tempo de execucao: %.03f segundos\n", tempo_gasto);

    freeMatriz(lin, col, matriz);
    fclose(file);
    free(jaVisitado);
    clearQueue(fila);
    free(fila);
    return 0;
}

nodo** funcMatriz(int linhas, int colunas) {
    nodo** mat = (nodo**)malloc(linhas * sizeof(nodo*));
    for (int l = 0; l < linhas; l++) {
        mat[l] = (nodo*)malloc(colunas * sizeof(nodo));
    }
    return mat;
}

void freeMatriz(int linhas, int colunas, nodo** matriz) {
    for (int l = 0; l < linhas; l++) {
        free(matriz[l]);
    }
    free(matriz);
}

void printMatriz(int linhas, int colunas, nodo** matriz) {
    printf("\n");
    for (int l = 0; l < linhas; l++) {
        for (int k = 0; k < colunas; k++) {
            printf("|%04d %c %d|", matriz[l][k].num, matriz[l][k].tipo, matriz[l][k].porto);
            if (matriz[l][k].norte)
                printf(" norte-> |%04d %c|", matriz[l][k].norte->num, matriz[l][k].norte->tipo);
            if (matriz[l][k].sul)
                printf(" sul-> |%04d %c|", matriz[l][k].sul->num, matriz[l][k].sul->tipo);
            if (matriz[l][k].leste)
                printf(" leste-> |%04d %c|", matriz[l][k].leste->num, matriz[l][k].leste->tipo);
            if (matriz[l][k].oeste)
                printf(" oeste-> |%04d %c|", matriz[l][k].oeste->num, matriz[l][k].oeste->tipo);
            printf("\n");
        }
        printf("\n");
    }
}

nodo* insereMatriz(int linhas, int colunas, nodo** matriz, FILE* file) {
    nodo* inicio = NULL;
    char temp;
    int i = 0;
    for (int l = 0; l < linhas; l++) {
        for (int k = 0; k < colunas; k++) {
            fscanf(file, " %c", &temp);
            matriz[l][k].num = i++;
            matriz[l][k].norte = NULL;
            matriz[l][k].sul = NULL;
            matriz[l][k].leste = NULL;
            matriz[l][k].oeste = NULL;
            switch (temp) {
            case '.':
                matriz[l][k].porto = 0;
                matriz[l][k].tipo = 'c';
                break;
            case '*':
                matriz[l][k].porto = 0;
                matriz[l][k].tipo = 'i';
                break;
            default:
                matriz[l][k].porto = temp - '0';
                if (temp - '0' == 1) inicio = &(matriz[l][k]);
                matriz[l][k].tipo = 'p';
                break;
            }
        }
    }
    for (int l = 0; l < linhas; l++) {
        for (int k = 0; k < colunas; k++) {
            if (matriz[l][k].tipo != 'i') {
                if (l > 0 && matriz[l - 1][k].tipo != 'i')
                    matriz[l][k].norte = &matriz[l - 1][k];
                if (l < linhas - 1 && matriz[l + 1][k].tipo != 'i')
                    matriz[l][k].sul = &matriz[l + 1][k];
                if (k > 0 && matriz[l][k - 1].tipo != 'i')
                    matriz[l][k].oeste = &matriz[l][k - 1];
                if (k < colunas - 1 && matriz[l][k + 1].tipo != 'i')
                    matriz[l][k].leste = &matriz[l][k + 1];
            }
        }
    }
    return inicio;
}

void enqueueNode(queue* fila, nodo* nodo) {
    queueNode* newNode = createqueueNode(nodo);
    if (fila->rear == NULL) {
        fila->front = newNode;
        fila->rear = newNode;
    }
    else {
        fila->rear->next = newNode;
        fila->rear = newNode;
    }
}

int dequeueNode(queue* fila) {
    if (fila->front == NULL) {
        return -1;
    }
    queueNode* temp = fila->front;
    int num = temp->nodo->num;
    fila->front = fila->front->next;
    if (fila->front == NULL) {
        fila->rear = NULL;
    }
    free(temp);
    return num;
}

char typecheck(queue* fila) {
    if (fila->front == NULL) {
        return '\0';
    }
    else {
        return fila->front->nodo->tipo;
    }
}

struct queueNode* createqueueNode(nodo* nodo) {
    queueNode* temp = (queueNode*)malloc(sizeof(queueNode));
    if (!temp) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    temp->nodo = nodo;
    temp->next = NULL;
    return temp;
}

struct queue* createQueue() {
    struct queue* temp = (struct queue*)malloc(sizeof(struct queue));
    if (!temp) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    temp->front = NULL;
    temp->rear = NULL;
    return temp;
}

void clearQueue(queue* fila) {
    while (fila->front != NULL) {
        queueNode* temp = fila->front;
        fila->front = fila->front->next;
        free(temp);
    }
    fila->rear = NULL;
}

void BFS(queue* fila, nodo** mapa, int porto, _Bool* javisitado, nodo* inicio, int lin, int col) {
    int totalCombustivel = 0;
    int tamanho = lin * col;
    int portoAtual = porto;
    int* distTo = (int*)calloc(tamanho, sizeof(int));

    while (1) {
        // Reset estrutura de visitação e fila
        for (int i = 0; i < tamanho; i++) {
            javisitado[i] = false;
            distTo[i] = 0;
        }
        clearQueue(fila);
        enqueueNode(fila, inicio);
        javisitado[inicio->num] = true;

        _Bool achouProximoPorto = false;
        int proximoPorto = (portoAtual == 9) ? 1 : portoAtual + 1;

        while (fila->front != NULL) {
            nodo* atual = fila->front->nodo;
            dequeueNode(fila);

            if (atual->porto == proximoPorto) {
                totalCombustivel += distTo[atual->num];
                printf("Porto %d encontrado com combustivel %d\n", atual->porto, distTo[atual->num]);
                printf("Total de combustivel ate agora: %d\n", totalCombustivel);

                // Prepara para buscar o próximo porto
                inicio = atual;
                portoAtual = proximoPorto;
                achouProximoPorto = true;
                break;
            }

            if (atual->norte && !javisitado[atual->norte->num] && atual->norte->tipo != 'i') {
                javisitado[atual->norte->num] = true;
                enqueueNode(fila, atual->norte);
                distTo[atual->norte->num] = distTo[atual->num] + 1;
            }
            if (atual->sul && !javisitado[atual->sul->num] && atual->sul->tipo != 'i') {
                javisitado[atual->sul->num] = true;
                enqueueNode(fila, atual->sul);
                distTo[atual->sul->num] = distTo[atual->num] + 1;
            }
            if (atual->leste && !javisitado[atual->leste->num] && atual->leste->tipo != 'i') {
                javisitado[atual->leste->num] = true;
                enqueueNode(fila, atual->leste);
                distTo[atual->leste->num] = distTo[atual->num] + 1;
            }
            if (atual->oeste && !javisitado[atual->oeste->num] && atual->oeste->tipo != 'i') {
                javisitado[atual->oeste->num] = true;
                enqueueNode(fila, atual->oeste);
                distTo[atual->oeste->num] = distTo[atual->num] + 1;
            }
        }

        if (!achouProximoPorto) {
            printf("Porto %d nao e acessivel\n", portoAtual + 1);
            portoAtual++;
            // O inicio não muda, pois tentamos do último porto alcançado
        }
        if (portoAtual == 1 && achouProximoPorto) break;
    }
    printf("Total de combustivel necessario: %d\n", totalCombustivel);
    free(distTo);
}