/* ALUNOS: ANA GRAZIELA MONTEIRO DOS SANTOS e PAULO HENRIQUE RODRIGUES CORRÊA
 * DISCIPLINA: ALGORITIMOS E PROGRAMAÇÃO II
 * PROFESSOR: ANDERSON BESSA
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* Constantes */
#define ERROR_FILE_MSG "Nao foi possivel abrir o arquivo!\n"
#define INVALID_OPTION "Opcao invalida! Tente novamente!"
#define URL_TXTS "tests/"
#define URL_BINS "bins/"

/* Definicoes de tipo */
enum boolean
{
	FALSE = 0,
	TRUE = 1
};

/* Prototipos */
FILE *load(char frame[9][9]);
FILE *load_continue_game(char frame[9][9], char *file_name);
void load_new_game(char frame[9][9], char *file_name);
FILE *create_binary_file(char frame[9][9]);
int determine_quadrant(int x, int y);
int is_valid(const char frame[9][9], int x, int y, int value);
int is_valid_column(const char frame[9][9], int y, int value);
int is_valid_line(const char frame[9][9], int x, int value);
int is_valid_quadrant3x3(const char frame[9][9], int x, int y, int value);
int exist_empty_position(const char frame[9][9]);
void print(const char frame[9][9]);
void play();
void solution_one_step(char frame[9][9]);
void complete_solution(FILE *, char frame[9][9]);
void save_bin_move(FILE *fb, char frame[9][9]);

/* Funcoes auxiliares */
int end_x(int quadrant);
int end_y(int quadrant);
int read_option();
void gen_random(char *s, const int len);
int ini_x(int quadrant);
int ini_y(int quadrant);
void menu();
void file_menu();

/* /////////////////////////////////////////////////////////////////////////////
 * MAIN
 * /////////////////////////////////////////////////////////////////////////////
 */
int main()
{
	// inicia o jogo
	play();

	return 0;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Inicializa o SUDOKU a partir de um novo jogo ou estado de jogo anterior
 * Não mexer, está fuincionando corretamente 
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
FILE *load(char frame[9][9])
{
	char folder_txt[50] = URL_TXTS, file_name_txt[20], folder_bin[50] = URL_BINS, file_name_bin[20];
	FILE *f = NULL;
	int option;

	file_menu();
	option = read_option();

	switch (option)
	{

	// carrega um novo sudoku
	case 1:
		printf("Digite o nome do arquivo de jogo (.txt): ");
		scanf("%s", file_name_txt);
		strcat(folder_txt, file_name_txt);

		// abre um novo arquivo.txt
		load_new_game(frame, folder_txt);

		break;

	// continuar jogo
	case 2:
		printf("Digite o nome do arquivo binário salvo (.bin): ");
		scanf("%s", file_name_bin);
		strcat(folder_bin, file_name_bin);

		// abre o arquivo binario já existente

		f = load_continue_game(frame, folder_bin);
		return f;

	// retornar ao menu anterior
	case 9:
		break;

	default:
		printf(INVALID_OPTION);
		return load(frame);
	}
	return f;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Carrega um estado de jogo a partir de um arquivo binario
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
FILE *load_continue_game(char frame[9][9], char *file_name)
{
	FILE *f;
	f = fopen(file_name, "rb+");
	if (f == NULL)
	{
		printf(ERROR_FILE_MSG);
	}
	else
	{	
		int numbers_of_plays;
		fseek(f, 0, SEEK_SET);
		fread(&numbers_of_plays, sizeof(int), 1, f);

		printf("Número total de jogadas: %d\n", numbers_of_plays);

		fseek(f, 4, SEEK_SET);
		// ler o estado salvo do jogo do arquivo binario
		fread(frame, sizeof(char), 81, f);
	}
	return f;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Carrega um novo jogo do Sudoku
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void load_new_game(char frame[9][9], char *file_name)
{
	FILE *f;
	f = fopen(file_name, "r");
	if (f == NULL)
		printf(ERROR_FILE_MSG);
	else
		for (int i = 0; i < 9; i++)
			for (int j = 0; j < 9; j++)
				fscanf(f, "%d", (int *)&frame[i][j]);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Cria arquivo binario
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
FILE *create_binary_file(char frame[9][9])
{
	int numbers_plays = 0;

	char folder_bin[20] = URL_BINS, file_name_bin[60];

	gen_random(file_name_bin, 5);

	strcat(folder_bin, file_name_bin);
	strcat(folder_bin, ".bin");

	FILE *fb = fopen(folder_bin, "wb+");

	if (fb == NULL)
	{
		printf(ERROR_FILE_MSG);
		return NULL;
	}
	else
	{
		fseek(fb, 0, SEEK_SET);
		fwrite(&numbers_plays, sizeof(int), 1, fb);

		fseek(fb, 4, SEEK_SET);
		fwrite(frame, sizeof(char), 81, fb);
	}
	return fb;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Dado as posicoes x e y, determina o quadrante do quadro
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int determine_quadrant(int x, int y)
{
	if (x < 3 && y < 3)
		return 1;
	else if (x < 3 && y < 6)
		return 2;
	else if (x < 3 && y < 9)
		return 3;
	else if (x < 6 && y < 3)
		return 4;
	else if (x < 6 && y < 6)
		return 5;
	else if (x < 6 && y < 9)
		return 6;
	else if (x < 9 && y < 3)
		return 7;
	else if (x < 9 && y < 6)
		return 8;
	else
		return 9;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Determina se um valor na posicao x e y eh valido
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int is_valid(const char frame[9][9], int x, int y, int value)
{
	if (!is_valid_column(frame, y, value))
		return FALSE;
	if (!is_valid_line(frame, x, value))
		return FALSE;
	if (!is_valid_quadrant3x3(frame, x, y, value))
		return FALSE;

	return TRUE;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Verifica se o valor na coluna y eh valido
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int is_valid_column(const char frame[9][9], int y, int value)
{
	for (int i = 0; i < 9; i++)
		if (frame[i][y] == value)
			return FALSE;
	return TRUE;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Verifica se o valor na linha x eh valido
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int is_valid_line(const char frame[9][9], int x, int value)
{
	for (int i = 0; i < 9; i++)
		if (frame[x][i] == value)
			return FALSE;
	return TRUE;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Verifica se o valor eh valido no quadrante da posicao x, y
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int is_valid_quadrant3x3(const char frame[9][9], int x, int y, int value)
{
	int d_q = determine_quadrant(x, y);

	for (int l = ini_x(d_q); l <= end_x(d_q); l++)
		for (int c = ini_y(d_q); c <= end_y(d_q); c++)
			if (frame[l][c] == value)
				return FALSE;
	return TRUE;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Verifica se existe um campo nao preenchido
 * Não mexer, está fuincionando corretamente 
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int exist_empty_position(const char frame[9][9])
{
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			if (frame[i][j] == 0)
				return TRUE;
	return FALSE;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Imprime o quadro recebido do sudoku
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void print(const char frame[9][9])
{
	puts("    0 1 2   3 4 5   6 7 8");
	for (int i = 0; i < 9; i++)
	{
		if (i % 3 == 0)
			puts("  -------------------------");
		for (int j = 0; j < 9; j++)
		{

			if (j == 0)
				printf("%d | ", i);
			else if (j % 3 == 0)
				printf("| ");

			if (frame[i][j] == 0)
				printf("- ");
			else
				printf("%d ", frame[i][j]);
		}
		puts("|");
	}
	puts("  -------------------------");
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Realiza toda a logica do jogo
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void play()
{
	int option;
	char frame[9][9] = {
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0}
	};
	FILE *fb = NULL;

	option = 0;

	while (option != 9)
	{
		// imprima na tela o quadro atual
		print(frame);

		// apresente um menu com as opcoes
		menu();
		option = read_option();

		switch (option)
		{

		// carregue sudoku
		case 1:
		{
			fb = load(frame);
			if (fb == NULL)
				fb = create_binary_file(frame);

			break;
		}

		// preenche quadro com um valor
		case 2:
		{
			int x, y, value;

			printf("Entre com a posicao e o valor (linha, coluna, valor): ");
			scanf("%d %d %d", &x, &y, &value);

			if (is_valid(frame, x, y, value))
			{
				frame[x][y] = value;
				save_bin_move(fb, frame);
			}
			else
				puts("Valor ou posicao incorreta! Tente novamente!");
		}
		break;

		// resolve um passo
		case 3:
		{
			solution_one_step(frame);
			save_bin_move(fb, frame);
			puts("Um passo resolvido!");
			break;
		}

		// resolve o sudoku completo
		case 4:{
			int numbers_of_plays;

			complete_solution(fb, frame);
			
			fseek(fb, 0, SEEK_SET);
			fread(&numbers_of_plays, sizeof(int), 1, fb);
			
			printf("Número total de jogadas: %d\n", numbers_of_plays);
			
			break;
		}

		case 9:
			fclose(fb);
			puts("Programa finalizado ..");
			break;

		default:
			puts(INVALID_OPTION);
			break;
		}
	}
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Resolve o sudoku
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void complete_solution(FILE *fb, char frame[9][9])
{
	while (exist_empty_position(frame))
	{
		solution_one_step(frame);
		save_bin_move(fb, frame);
	}
	
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Preenche apenas um campo vazio
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void solution_one_step(char frame[9][9])
{
	for (int l = 0; l < 9; l++)
	{
		for (int c = 0; c < 9; c++)
		{
			// Os dois lacos acima é para percorrer todas as posicoes do quadro
			/* m = Número de valores que podem ser inseridos
			 * x, y, e v são variaveis auxiliares
			 */

			int m, v;
			m = v = 0;

			if (frame[l][c] == 0)
			{
				// Para verificar quais valores podem ser inseridos na posicao quadro[l][c]
				for (int k = 1; k < 10; k++)
				{
					if (is_valid(frame, l, c, k))
					{
						m++;
						v = k;
					}

					if (m > 1)
						break;
				}
				if (m == 1)
				{
					frame[l][c] = v;
					return;
				}
			}
		}
	}
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Salva o estado atual do quadro no arquivo binario
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void save_bin_move(FILE *fb, char frame[9][9])
{
	if (fb == NULL)
	{
		printf("Erro: O arquivo não foi aberto corretamente.\n");
		return;
	}

	// // Leh o numero atual de jogadas no inicio do arquivo
	int numbers_of_plays = 0;
	fseek(fb, 0, SEEK_SET);
	fread(&numbers_of_plays, sizeof(int), 1, fb);

	// // Incrementa o numero de jogadas
	numbers_of_plays++;

	// // Posiciona o ponteiro no inicio para atualizar o numero de jogadas
	fseek(fb, 0, SEEK_SET);
	fwrite(&numbers_of_plays, sizeof(int), 1, fb);

	// // Move o ponteiro para o final do arquivo para adicionar o quadro atual
	fseek(fb, 4, SEEK_SET);
	fwrite(frame, sizeof(char), 81, fb);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 							FUNCOES AUXILIARES
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Indice final da linha para o quadrante recebido como parametro
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int end_x(int quadrant)
{
	switch (quadrant)
	{
	case 1:
	case 2:
	case 3:
		return 2;

	case 4:
	case 5:
	case 6:
		return 5;

	default:
		return 8;
	}
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Indice final da coluna para o quadrante recebido como parametro
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int end_y(int quadrant)
{
	switch (quadrant)
	{
	case 1:
	case 4:
	case 7:
		return 2;

	case 2:
	case 5:
	case 8:
		return 5;

	default:
		return 8;
	}
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Gera uma cadeia de caracteres randomica de tamanho len
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void gen_random(char *s, const int len)
{
	srand(time(NULL));
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	int i;

	for (i = 0; i < len; ++i)
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	s[len] = 0;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Indice inicial da linha para o quadrante recebido como parametro
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int ini_x(int quadrant)
{
	switch (quadrant)
	{
	case 1:
	case 2:
	case 3:
		return 0;

	case 4:
	case 5:
	case 6:
		return 3;

	default:
		return 6;
	}
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Indice inicial da coluna para o quadrante recebido como parametro
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int ini_y(int quadrant)
{
	switch (quadrant)
	{
	case 1:
	case 4:
	case 7:
		return 0;

	case 2:
	case 5:
	case 8:
		return 3;

	default:
		return 6;
	}
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Imprime a mensagem a faz a leitura da opcao
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int read_option()
{
	int options;

	printf("Opcao: ");
	scanf("%d", &options);

	return options;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Imprime o menu de opcoes
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void menu()
{
	puts("\n~~~~~~~~ SUDOKU ~~~~~~~~");
	puts("[1] - Carregar jogo");
	puts("[2] - Jogar");
	puts("[3] - Resolver um passo");
	puts("[4] - Resolver");
	puts("[9] - Finalizar");
	puts("--------");
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Imprime o menu de opcoes do arquivo
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void file_menu()
{
	puts("\n~~~~~ MENU ARQUIVO ~~~~~");
	puts("[1] - Novo jogo");
	puts("[2] - Continuar jogo");
	puts("[9] - Retornar ao menu anterior");
	puts("--------");
}
