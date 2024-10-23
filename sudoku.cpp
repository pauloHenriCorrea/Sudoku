#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* Constantes */
#define ERROR_FILE_MSG "Nao foi possivel abrir o arquivo!\n"
#define INVALID_OPTION "Opcao invalida! Tente novamente!"

/* Definicoes de tipo */
enum boolean
{
	FALSO = 0,
	VERDADEIRO = 1
};

/* Prototipos */
FILE *carregue(char quadro[9][9]);
FILE *carregue_continue_jogo(char quadro[9][9], char *nome_arquivo);
void carregue_novo_jogo(char quadro[9][9], char *nome_arquivo);
FILE *crie_arquivo_binario(char quadro[9][9]);
int determine_quadrante(int x, int y);
int eh_valido(const char quadro[9][9], int x, int y, int valor);
int eh_valido_na_coluna(const char quadro[9][9], int y, int valor);
int eh_valido_no_quadrante3x3(const char quadro[9][9], int x, int y, int valor);
int eh_valido_na_linha(const char quadro[9][9], int x, int valor);
int existe_posicao_vazia(const char quadro[9][9]);
void imprima(const char quadro[9][9]);
void jogue();
void resolve_completo(FILE *, char quadro[9][9]);
void resolve_um_passo(char quadro[9][9]);
void salve_jogada_bin(FILE *fb, char quadro[9][9]);

/* Funcoes auxiliares */
int fim_x(int quadr);
int fim_y(int quadr);
int leia_opcao();
void gen_random(char *s, const int len);
int ini_x(int quadr);
int ini_y(int quadr);
void menu();
void menu_arquivo();

/* -----------------------------------------------------------------------------
 * -----------------------------------------------------------------------------
 * MAIN
 * /////////////////////////////////////////////////////////////////////////////
 */
int main()
{
	// inicia o jogo
	jogue();

	return 0;
}

/* -----------------------------------------------------------------------------
 * CARREGAR
 * Inicializa o SUDOKU a partir de um novo jogo ou estado de jogo anterior
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * REVIZAR ESTÁ FUNÇÃO PARA VER SE ESTÁ FUNCIONANDO CORRETAMENTE
 */
// FINALIZADO
FILE *carregue(char quadro[9][9])
{
	char url[50] = "exemplos_teste/", file_name[20], url2[50] = "binarios/", file_name2[20];
	FILE *f = NULL;
	int opcao;

	menu_arquivo();
	opcao = leia_opcao();

	switch (opcao)
	{

	// carregar novo sudoku
	case 1:
		// FINALIZADO
		printf("Digite o nome do arquivo de jogo (.txt): ");
		scanf("%s", file_name);
		strcat(url, file_name);

		// abre um novo arquivo.txt
		carregue_novo_jogo(quadro, url);

		break;

	// continuar jogo
	case 2:
		// FINALIZADO
		printf("Digite o nome do arquivo binário salvo (.bin): ");
		scanf("%s", file_name2);
		strcat(url2, file_name2);

		// abre o arquivo binario já existente
		f = carregue_continue_jogo(quadro, url2);
		return f;
	// retornar ao menu anterior
	case 9:
		break;

	default:
		printf(INVALID_OPTION);
		return carregue(quadro);
	}
	return f;
}

/* -----------------------------------------------------------------------------
 * CARREGAR CONTINUAR JOGO
 * Carrega um estado de jogo a partir de um arquivo binario
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

// FINALIZADO
FILE *carregue_continue_jogo(char quadro[9][9], char *nome_arquivo)
{
	FILE *f;
	f = fopen(nome_arquivo, "rb");
	if (f == NULL)
	{
		printf(ERROR_FILE_MSG);
	}
	else
	{
		// ler o estado salvo do jogo do arquivo binario
		fread(quadro, sizeof(char), 81, f);
	}
	return f;
}

/* -----------------------------------------------------------------------------
 * CARREGAR NOVO JOGO
 * Carrega um novo jogo do Sudoku
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

// FINALIZADO
void carregue_novo_jogo(char quadro[9][9], char *nome_arquivo)
{
	// TODO
	FILE *f;
	f = fopen(nome_arquivo, "r");
	if (f == NULL)
		printf(ERROR_FILE_MSG);
	else
		for (int i = 0; i < 9; i++)
			for (int j = 0; j < 9; j++)
				fscanf(f, "%d", (int *)&quadro[i][j]);
}

/* -----------------------------------------------------------------------------
 * CRIAR ARQUIVO BINARIO
 * Criar arquivo binario
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
FILE *crie_arquivo_binario(char quadro[9][9])
{
	char url[20], address[60] = "binarios/";
	gen_random(url, 5);
	strcat(address, url);
	strcat(address, ".bin");

	FILE *fb = fopen(address, "wb");

	if (fb == NULL)
	{
		printf(ERROR_FILE_MSG);
		return NULL;
	}
	else
	{
		fwrite(quadro, sizeof(char), 9 * 9, fb);
		fclose(fb);
	}
	return fb;
}

/* -----------------------------------------------------------------------------
 * DETERMINAR QUADRANTE
 * Dado as posicoes x e y, determina o quadrante do quadro
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int determine_quadrante(int x, int y)
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

/* -----------------------------------------------------------------------------
 * E VALIDO
 * Determina se um valor na posicao x e y eh valido
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int eh_valido(const char quadro[9][9], int x, int y, int valor)
{
	// verifica as tres condicoes
	if (!eh_valido_na_coluna(quadro, y, valor))
		return FALSO;
	if (!eh_valido_na_linha(quadro, x, valor))
		return FALSO;
	if (!eh_valido_no_quadrante3x3(quadro, x, y, valor))
		return FALSO;

	return VERDADEIRO;
}

/* -----------------------------------------------------------------------------
 * E VALIDO NA COLUNA
 * Verifica se um valor na coluna y e valido
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int eh_valido_na_coluna(const char quadro[9][9], int y, int valor)
{
	for (int i = 0; i < 9; i++)
		if (quadro[i][y] == valor)
			return FALSO;
	return VERDADEIRO;
}

/* -----------------------------------------------------------------------------
 * E VALIDO NA LINHA
 * Verifica se um valor na linha x e valido
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int eh_valido_na_linha(const char quadro[9][9], int x, int valor)
{
	for (int i = 0; i < 9; i++)
		if (quadro[x][i] == valor)
			return FALSO;
	return VERDADEIRO;
}

/* -----------------------------------------------------------------------------
 * E VALIDO NO QUADRO 3X3
 * Verifica se um valor e valido no quadrante da posicao x, y
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int eh_valido_no_quadrante3x3(const char quadro[9][9], int x, int y, int valor)
{
	int d_q = determine_quadrante(x, y);
	int inicio_l = ini_x(d_q), inicio_c = ini_y(d_q), fim_l = fim_x(d_q), fim_c = fim_y(d_q);
	// Verefica se o determinado valor pode ser insiredo na posicao desejada
	for (int l = inicio_l; l <= fim_l; l++)
		for (int c = inicio_c; c <= fim_c; c++)
			if (quadro[l][c] == valor)
				return FALSO;
	return VERDADEIRO;
}

/* -----------------------------------------------------------------------------
 * EXISTE CAMPO VAZIO
 * Verifica se existe um campo nao preenchido
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int existe_posicao_vazia(const char quadro[9][9])
{
	int i, j;

	for (i = 0; i < 9; i++)
		for (j = 0; j < 9; j++)
			if (quadro[i][j] == 0)
				return VERDADEIRO;
	return FALSO;
}

/* -----------------------------------------------------------------------------
 * IMPRIMIR
 * Imprime o quadro recebido do sudoku
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void imprima(const char quadro[9][9])
{
	int i, j;

	//	puts("~~~~~~~~ SUDOKU ~~~~~~~~");
	puts("    0 1 2   3 4 5   6 7 8");
	for (i = 0; i < 9; i++)
	{
		if (i % 3 == 0)
			puts("  -------------------------");
		for (j = 0; j < 9; j++)
		{

			if (j == 0)
				printf("%d | ", i);
			else if (j % 3 == 0)
				printf("| ");

			if (quadro[i][j] == 0)
				printf("- ");
			else
				printf("%d ", quadro[i][j]);
		}
		puts("|");
	}
	puts("  -------------------------");
}

/* -----------------------------------------------------------------------------
 * JOGAR
 * Realiza toda a logica do jogo
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void jogue()
{
	int opcao;
	char quadro[9][9] = {
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0}};
	FILE *fb = NULL;

	opcao = 0;

	while (opcao != 9)
	{
		// imprima na tela o quadro atual
		imprima(quadro);

		// apresente um menu com as opcoes
		menu();
		opcao = leia_opcao();

		switch (opcao)
		{

		// carregue sudoku
		case 1:
			// FINALIZADO
			fb = carregue(quadro);
			if (fb == NULL)
				fb = crie_arquivo_binario(quadro);

			break;

		// preencha quadro com um valor
		case 2:
		{
			int x, y, valor;

			printf("Entre com a posicao e o valor (linha, coluna, valor): ");
			scanf("%d %d %d", &x, &y, &valor);

			if (eh_valido(quadro, x, y, valor))
			{
				quadro[x][y] = valor;
				salve_jogada_bin(fb, quadro);
			}
			else
				puts("Valor ou posicao incorreta! Tente novamente!");
		}
		break;

		// resolva 1 passo
		case 3:
			resolve_um_passo(quadro);
			salve_jogada_bin(fb, quadro);
			puts("Um passo resolvido!");
			break;

		// resolva o sudoku completo
		case 4:
			resolve_completo(fb, quadro);
			break;

		case 9:
			puts("Programa finalizado ..");
			break;

		default:
			puts(INVALID_OPTION);
			break;
		}
	}
}

/* -----------------------------------------------------------------------------
 * RESOLVER
 * Resolve o sudoku
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void resolve_completo(FILE *fb, char quadro[9][9])
{
	while (existe_posicao_vazia(quadro))
	{
		resolve_um_passo(quadro);
		salve_jogada_bin(fb, quadro);
	}
}

/* -----------------------------------------------------------------------------
 * RESOLVER UM PASSO
 * Preenche apenas um campo vazio
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void resolve_um_passo(char quadro[9][9])
{
	// TODO
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
			
			if (quadro[l][c] == 0)
			{
				// Para verificar quais valores podem ser inseridos na posicao quadro[l][c]
				for (int k = 1; k < 10; k++)
				{
					if (eh_valido(quadro, l, c, k))
					{
						m++;
						v = k;
					}

					if (m > 1)
						break;
				}
				if (m == 1)
				{
					quadro[l][c] = v;
					return;
				}
			}
		}
	}
}

/* -----------------------------------------------------------------------------
 * SALVAR JOGADA BINARIO
 * Salva o estado atual do quadro no arquivo binario
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void salve_jogada_bin(FILE *fb, char quadro[9][9])
{
	// TODOs
	fseek(fb, 0, SEEK_SET);
	fwrite(quadro, sizeof(char), 81, fb);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 							FUNCOES AUXILIARES
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/* -----------------------------------------------------------------------------
 * FIM X
 * Indice final da linha para o quadrante recebido como parametro
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int fim_x(int quadrante)
{
	switch (quadrante)
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

/* -----------------------------------------------------------------------------
 * FIM Y
 * Indice final da coluna para o quadrante recebido como parametro
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int fim_y(int quadrante)
{
	switch (quadrante)
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

/* -----------------------------------------------------------------------------
 * GEN_RANDOM
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

/* -----------------------------------------------------------------------------
 * INI X
 * Indice inicial da linha para o quadrante recebido como parametro
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int ini_x(int quadrante)
{
	switch (quadrante)
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

/* -----------------------------------------------------------------------------
 * INI Y
 * Indice inicial da coluna para o quadrante recebido como parametro
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int ini_y(int quadrante)
{
	switch (quadrante)
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

/* -----------------------------------------------------------------------------
 * LE OPCAO
 * Imprime a mensagem a faz a leitura da opcao
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int leia_opcao()
{
	int opcao;

	printf("Opcao: ");
	scanf("%d", &opcao);

	return opcao;
}

/* -----------------------------------------------------------------------------
 * MENU
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

/* -----------------------------------------------------------------------------
 * MENU ARQUIVO
 * Imprime o menu de opcoes do arquivo
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void menu_arquivo()
{
	puts("\n~~~~~ MENU ARQUIVO ~~~~~");
	puts("[1] - Novo jogo");
	puts("[2] - Continuar jogo");
	puts("[9] - Retornar ao menu anterior");
	puts("--------");
}
