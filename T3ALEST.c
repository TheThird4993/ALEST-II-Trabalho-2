#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


typedef struct nodo nodo;
typedef struct queueNode queueNode;
typedef struct queue queue;


struct nodo {
	nodo *norte, *sul, *leste, *oeste;
	char tipo;                        // se for 1-9 é um "porto", se não, é um caminho. '*' não são contabilizados como "nodos".
	int porto;                  // se for um porto, o numero do porto, se não, é 0
	int num;
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

struct queue* clearQueue(queue*);

struct queue* createQueue();

void enqueueNode(queue*, nodo*);

int dequeueNode(queueNode*);

char const typecheck(queueNode*);

nodo** funcMatriz(int, int);

void freeMatriz(int, int, nodo**);

void printMatriz(int, int, nodo**);

struct nodo* insereMatriz(int, int, nodo**, FILE*);

void BFS(struct queueNode* queue,nodo** mapa, int porto, _Bool*, nodo* inicio, int linha, int coluna);

int main() {
	int lin, col;
	nodo **matriz;
	FILE *file;

	file = fopen("case0.map", "r");

	if (!file) {
		perror("Erro ao abrir arquivo");
		return 1;
	}

//-------------------------------------------  para ler o arquivo
	int c;
	while ((c = fgetc(file)) != EOF) {
		printf("%c", c);
	}
	printf("\n");
	rewind(file);                            // para retornar o ponteiro de leitura ao inicio do arquivo
//-------------------------------------------

	fscanf(file, "%d %d", &lin, &col);

	matriz = funcMatriz(lin, col);

	printf("linhas = %d\ncolunas = %d\n", lin, col);
	nodo* nodoInicial = insereMatriz(lin, col, matriz, file); // insere os nodos na matriz e retorna o nodo do primeiro porto
	//-------------------------------------------  algoritmo para determinha "combustivel" total necessario

	int tamanho = lin * col;
	_Bool* jaVisitado = (_Bool*)calloc(tamanho,sizeof(_Bool));
	struct queue* queue = createQueue();
	enqueueNode(queue, nodoInicial); // inicia a fila com o primeiro porto encontrado

	BFS(queue, matriz, nodoInicial->porto, jaVisitado, nodoInicial, lin, col);

	printMatriz(lin, col, matriz);

	freeMatriz(lin, col, matriz);
	fclose(file);
	free(jaVisitado);
	return 0;
}

nodo** funcMatriz(int linhas, int colunas) {
	nodo**mat;

	mat = (nodo**)malloc(linhas * sizeof(nodo*));

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

struct nodo* insereMatriz(int linhas, int colunas, nodo** matriz, FILE* file) {
	nodo* inicio = NULL;
	char temp;
	int i = 0;
	for (int l = 0; l < linhas; l++) {
		for (int k = 0; k < colunas; k++) {
			fscanf(file, " %c", &temp);
			matriz[l][k].num   = i++;
			//-------------------------- inicialização dos ponteiros
			matriz[l][k].norte = NULL;
			matriz[l][k].sul   = NULL;
			matriz[l][k].leste = NULL;
			matriz[l][k].oeste = NULL;
			//--------------------------
			switch (temp) {
			case '.' :
				matriz[l][k].porto = 0; // '.' não é um porto, mas sim um caminho
				matriz[l][k].tipo = 'c';
				break;
			case '*':
				matriz[l][k].porto = 0; // '*' não é nodo navegavel
				matriz[l][k].tipo = 'i';
				break;
			default:
				matriz[l][k].porto = temp - '0'; // converte para int e guarda o char
				if (temp - '0' == 1) inicio = &(matriz[l][k]); // guarda o primeiro porto encontrado
				matriz[l][k].tipo = 'p';
				break;
			}
		}
	}
	for (int l = 0; l < linhas; l++) {
		for (int k = 0; k < colunas; k++) {
			if (matriz[l][k].tipo != 'i') {
				// Norte
				if (l > 0 && matriz[l - 1][k].tipo != 'i')
					matriz[l][k].norte = &matriz[l - 1][k];

				// Sul
				if (l < linhas - 1 && matriz[l + 1][k].tipo != 'i')
						matriz[l][k].sul = &matriz[l + 1][k];

				// Oeste
				if (k > 0 && matriz[l][k - 1].tipo != 'i')
							matriz[l][k].oeste = &matriz[l][k - 1];

				// Leste
				if (k < colunas - 1 && matriz[l][k + 1].tipo != 'i')
					matriz[l][k].leste = &matriz[l][k + 1];
			}
		}
	}
	return inicio;
}
void enqueueNode(struct queue* queue, nodo* nodo) {
	if (queue->rear == NULL) {
		queue->front = createqueueNode(nodo);
		queue->rear = queue->front;
	}
	else {
		queueNode* newNode = createqueueNode(nodo);
		queue->rear->next = newNode;
		queue->rear = newNode;
	}
}

int dequeueNode(struct queue* queue) {
	if (queue->front == NULL) {
		return -1; // Queue está vazia
	}
	queueNode* temp = queue->front;
	int num = temp->nodo->num; // guarda o numero do nodo
	queue->front = queue->front->next;
	if (queue->front == NULL) {
		queue->rear = NULL; // se a fila ficou vazia, rear também deve ser NULL
	}
	free(temp);
	return num; // retorna o numero do nodo removido
}

char const typecheck (struct queue* queue) {
	char tipo;
	if (queue->front == NULL) {
		return NULL; // Queue está vazia
	}
	else {
		tipo = queue->front->nodo->tipo; // guarda o tipo do nodo
	}
	return tipo;
}
struct queueNode* createqueueNode(nodo* nodo) {
	struct queueNode* temp = NULL;
	temp = (struct queueNode*)malloc(sizeof(struct queueNode));
	if (!temp) {
		fprintf(stderr, "Memory allocation failed\n");
		exit(EXIT_FAILURE);
	}
	temp->nodo = nodo;
	temp->next = NULL;
	return temp;
}

struct queue* createQueue() {
	struct queue* temp = NULL;
	temp = (struct queue*)malloc(sizeof(struct queue));
	if (!temp) {
		fprintf(stderr, "Memory allocation failed\n");
		exit(EXIT_FAILURE);
	}
	temp->front = NULL;
	temp->rear = NULL;
	return temp;
}

struct queue* clearQueue(queue* queue) {
	while (queue->front != NULL) {
		queueNode* temp = queue->front;
		queue->front = queue->front->next;
		free(temp);
	}
}

void BFS(queue* queue, nodo** mapa, int porto, _Bool* javisitado, nodo* inicio, int lin, int col) {
	int totalCombustivel = 0, combustivelUsado = 0;
	int tamanho = lin * col;
	int portoAtual = porto; // pega o porto inicial
	int* distTo = (int*)calloc(tamanho, sizeof(int));
	javisitado[inicio->num] = true; // marca o nodo inicial como visitado
	nodo* temp = inicio; // guarda um nodo temporario
	while (queue->front != NULL && porto < 9) {
		combustivelUsado++;
		nodo* atual = queue->front->nodo; // pega o nodo atual da fila
		int numAtual = dequeueNode(queue); // remove o nodo da fila
		if (atual->porto == porto + 1 ) { // se for o proximo porto
				totalCombustivel += distTo[atual->num]; // soma o combustivel usado
				printf("Porto %d encontrado com combustivel %d\n", atual->porto, distTo[atual->num]);
				printf("Total de combustivel ate agora: %d\n", totalCombustivel);
				enqueueNode(queue, atual); // re-adiciona o nodo atual na fila para continuar a busca
				temp = atual; // atualiza o nodo temporario
				portoAtual = porto + 1;
			porto++;
			for (int i = 0; i < tamanho; i++) {
				javisitado[i] = false; // reseta o vetor de visitados
				distTo[i] = 0; // reseta a distancia
			}
			javisitado[inicio->num] = true; // marca o nodo inicial como visitado
			
		}
		if (atual->norte && !javisitado[atual->norte->num] && atual->norte->tipo != "i") {
			javisitado[atual->norte->num] = true; // marca como visitado
			enqueueNode(queue, atual->norte); // adiciona o nodo norte na fila
			distTo[atual->norte->num] = distTo[atual->num] + 1; // atualiza a distancia
		}
		if (atual->sul && !javisitado[atual->sul->num] && atual->sul->tipo != "i") {
			javisitado[atual->sul->num] = true; // marca como visitado
			enqueueNode(queue, atual->sul); // adiciona o nodo sul na fila
			distTo[atual->sul->num] = distTo[atual->num] + 1; // atualiza a distancia
		}
		if (atual->leste && !javisitado[atual->leste->num] &&atual->leste->tipo != "i") {
			javisitado[atual->leste->num] = true; // marca como visitado
			enqueueNode(queue, atual->leste); // adiciona o nodo leste na fila
			distTo[atual->leste->num] = distTo[atual->num] + 1; // atualiza a distancia
		}
		if (atual->oeste && !javisitado[atual->oeste->num] && atual->oeste->tipo != "i") {
			javisitado[atual->oeste->num] = true; // marca como visitado
			enqueueNode(queue, atual->oeste); // adiciona o nodo oeste na fila
			distTo[atual->oeste->num] = distTo[atual->num] + 1; // atualiza a distancia
		}
		if (queue->front->next == NULL) {
			printf("Porto %d nao e acessivel\n", porto + 1);
			enqueueNode(queue, temp); // re-adiciona o nodo temporario na fila
		porto++;
		for (int i = 0; i < tamanho; i++) {
			javisitado[i] = false; // reseta o vetor de visitados
		}
		}
	}
	printf("Total de combustivel necessario: %d\n", totalCombustivel);
	free(distTo);
}