#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct nodo nodo;

struct nodo {
	nodo* norte, * sul, * leste, * oeste;   // ponteiros para cada direção
	char tipo;                           // se for 0-9 é um "porto", se for '.', é um caminho. '*' são contabilizados como "invalidos".
	unsigned int visitado : 1;				// bitfield de 1 bit para ser usado como flag, caso ja tenha sido visitado ou não  
	int num_porto;							// se for um porto, qual deles
	int num;								// teste
};

nodo** funcMatriz(int, int);

void freeMatriz(int, int, nodo**);

void printMatriz(int, int, nodo**);

void insereMatriz(int, int, nodo**, FILE*, int[2][9]);

void printPos(int[2][9]);

int main() {
	int lin, col;
	int pos[2][9] = {0};
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

	insereMatriz(lin, col, matriz, file, pos);
	printMatriz(lin, col, matriz);

	printPos(pos);

	freeMatriz(lin, col, matriz);
	fclose(file);
	return 0;
}

void printPos(int matriz[2][9]) {
		for (int k = 0; k < 9; k++) {
			printf("porto %d : [%02d] [%02d]\n", k+1, matriz[0][k], matriz[1][k]);
		}
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

void printMatriz(int linhas, int colunas, nodo** matriz) {				// função para testes
	printf("\n");
	for (int l = 0; l < linhas; l++) {
		for (int k = 0; k < colunas; k++) {
			printf("|%04d %c|", matriz[l][k].num, matriz[l][k].tipo);
			if (matriz[l][k].tipo == 'p')
				printf(" porto %d", matriz[l][k].num_porto);

			if (matriz[l][k].norte) {
				printf(" norte-> |%04d %c|", matriz[l][k].norte->num, matriz[l][k].norte->tipo);
				if (matriz[l][k].norte->tipo == 'p')
					printf(" porto %d", matriz[l][k].norte->num_porto);
			}

			if (matriz[l][k].sul) {
				printf(" sul-> |%04d %c|", matriz[l][k].sul->num, matriz[l][k].sul->tipo);
				if (matriz[l][k].sul->tipo == 'p')
					printf(" porto %d", matriz[l][k].sul->num_porto);
			}

			if (matriz[l][k].leste) {
				printf(" leste-> |%04d %c|", matriz[l][k].leste->num, matriz[l][k].leste->tipo);
				if (matriz[l][k].leste->tipo == 'p')
					printf(" porto %d", matriz[l][k].leste->num_porto);
			}

			if (matriz[l][k].oeste) {
				printf(" oeste-> |%04d %c|", matriz[l][k].oeste->num, matriz[l][k].oeste->tipo);
				if (matriz[l][k].oeste->tipo == 'p')
					printf(" porto %d", matriz[l][k].oeste->num_porto);
			}

			printf("\n");
		}
		printf("\n");
	}
}

void insereMatriz(int linhas, int colunas, nodo** matriz, FILE* file, int pos[2][9]) {
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
				matriz[l][k].tipo = 'c';
				break;
			case '*':
				matriz[l][k].tipo = 'i';
				break;
			default:
				matriz[l][k].tipo = 'p';
				matriz[l][k].num_porto = temp - '0';
				pos[0][(temp - '0') - 1] = l;
				pos[1][(temp - '0') - 1] = k;
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
}