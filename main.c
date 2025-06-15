#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct nodo nodo;

struct nodo {
	nodo* norte, * sul, * leste, * oeste;   // ponteiros para cada direção
	char tipo[2];                           // se for 0-9 é um "porto", se for '.', é um caminho. '*' são contabilizados como "invalidos".
	unsigned int visitado : 1;				// bitfield de 1 bit para ser usado como flag, caso ja tenha sido visitado ou não  
	int num_porto;							// se for um porto, qual deles
	int num;								// teste
};

nodo** funcMatriz(int, int);

void freeMatriz(int, int, nodo**);

void printMatriz(int, int, nodo**);

void insereMatriz(int, int, nodo**, FILE*);

int main() {
	int lin, col;
	nodo **matriz;
	FILE *file;

	file = fopen("teste.map", "r");

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

	insereMatriz(lin, col, matriz, file);
	printMatriz(lin, col, matriz);

	freeMatriz(lin, col, matriz);
	fclose(file);
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
			printf("|%04d %s|", matriz[l][k].num, matriz[l][k].tipo);
			if (strcmp(matriz[l][k].tipo, "p") == 1)
				printf(" porto %d", matriz[l][k].num_porto);

			if (matriz[l][k].norte) {
				printf(" norte-> |%04d %s|", matriz[l][k].norte->num, matriz[l][k].norte->tipo);
				if (strcmp(matriz[l][k].norte->tipo, "p") == 0)
					printf(" porto %d", matriz[l][k].norte->num_porto);
			}

			if (matriz[l][k].sul) {
				printf(" sul-> |%04d %s|", matriz[l][k].sul->num, matriz[l][k].sul->tipo);
				if (strcmp(matriz[l][k].sul->tipo, "p") == 0)
					printf(" porto %d", matriz[l][k].sul->num_porto);
			}

			if (matriz[l][k].leste) {
				printf(" leste-> |%04d %s|", matriz[l][k].leste->num, matriz[l][k].leste->tipo);
				if (strcmp(matriz[l][k].leste->tipo, "p") == 0)
					printf(" porto %d", matriz[l][k].leste->num_porto);
			}

			if (matriz[l][k].oeste) {
				printf(" oeste-> |%04d %s|", matriz[l][k].oeste->num, matriz[l][k].oeste->tipo);
				if (strcmp(matriz[l][k].oeste->tipo, "p") == 0)
					printf(" porto %d", matriz[l][k].oeste->num_porto);
			}

			printf("\n");
		}
		printf("\n");
	}
}

void insereMatriz(int linhas, int colunas, nodo** matriz, FILE* file) {
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
				strcpy(matriz[l][k].tipo, "c");
				break;
			case '*':
				strcpy(matriz[l][k].tipo, "i");
				break;
			default:
				strcpy(matriz[l][k].tipo, "p");
				matriz[l][k].num_porto = temp - '0';
				break;
			}
		}
	}
	for (int l = 0; l < linhas; l++) {
		for (int k = 0; k < colunas; k++) {
			if (strcmp(matriz[l][k].tipo, "i") != 0) {
				// Norte
				if (l > 0 && strcmp(matriz[l - 1][k].tipo, "i") != 0)
					matriz[l][k].norte = &matriz[l - 1][k];

				// Sul
				if (l < linhas - 1 && strcmp(matriz[l + 1][k].tipo, "i") != 0)
						matriz[l][k].sul = &matriz[l + 1][k];

				// Oeste
				if (k > 0 && strcmp(matriz[l][k - 1].tipo, "i") != 0)
							matriz[l][k].oeste = &matriz[l][k - 1];

				// Leste
				if (k < colunas - 1 && strcmp(matriz[l][k + 1].tipo, "i") != 0)
					matriz[l][k].leste = &matriz[l][k + 1];
			}
		}
	}
}