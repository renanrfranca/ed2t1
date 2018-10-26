/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 01
 *
 * RA: 489697
 * Aluno: Renan Rossignatti de França
 * ========================================================================== */

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY	11
#define TAM_NOME 		51
#define TAM_MARCA 		51
#define TAM_DATA 		11
#define TAM_ANO 		3
#define TAM_PRECO 		8
#define TAM_DESCONTO 	4
#define TAM_CATEGORIA 	51


#define TAM_REGISTRO 	192
#define MAX_REGISTROS 	1000
#define MAX_CATEGORIAS 	30
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)


/* Saídas para o usuario */
#define OPCAO_INVALIDA 				"Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE 		"Memoria insuficiente!"
#define REGISTRO_N_ENCONTRADO 		"Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO 				"Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA			"ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO 				"Arquivo vazio!"
#define INICIO_BUSCA 		 		"**********************BUSCAR**********************\n"
#define INICIO_LISTAGEM  			"**********************LISTAR**********************\n"
#define INICIO_ALTERACAO 			"**********************ALTERAR*********************\n"
#define INICIO_EXCLUSAO  			"**********************EXCLUIR*********************\n"
#define INICIO_ARQUIVO  			"**********************ARQUIVO*********************\n"
#define INICIO_ARQUIVO_SECUNDARIO	"*****************ARQUIVO SECUNDARIO****************\n"
#define SUCESSO  				 	"OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 	"FALHA AO REALIZAR OPERACAO!\n"



/* Registro do Produto */
typedef struct {
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char marca[TAM_MARCA];
	char data[TAM_DATA];	/* DD/MM/AAAA */
	char ano[TAM_ANO];
	char preco[TAM_PRECO];
	char desconto[TAM_DESCONTO];
	char categoria[TAM_CATEGORIA];
} Produto;


/*----- Registros dos Índices -----*/

/* Struct para índice Primário */
typedef struct primary_index{
  char pk[TAM_PRIMARY_KEY];
  int rrn;
} Ip;

/* Struct para índice secundário */
typedef struct secundary_index{
  char pk[TAM_PRIMARY_KEY];
  char string[TAM_NOME];
} Is;

/* Struct para índice secundário de preços */
typedef struct secundary_index_of_final_price{
  float price;
  char pk[TAM_PRIMARY_KEY];
} Isf;

/* Lista ligada para o Índice abaixo*/
typedef struct linked_list{
  char pk[TAM_PRIMARY_KEY];
  struct linked_list *prox;
} ll;

/* Struct para lista invertida */
typedef struct reverse_index{
  char cat[TAM_CATEGORIA];
  ll* lista;
} Ir;

/*----- GLOBAL -----*/
char ARQUIVO[TAM_ARQUIVO];

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

// ======================= ROTINAS DE INICIALIZAÇÃO =========================

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo();

/* (Re)faz o índice respectivo */
void criar_iprimary(Ip *indice_primario, int* nregistros);

/* (Re)faz o índice respectivo */
void criar_secundarios(Is *iproduct, Is *ibrand, Ir *icategory, Isf *iprice, int nregistros, int *ncat);

// ========================== ROTINAS DE LEITURA ============================

/* Recupera do arquivo o registro com o rrn informado
 *  e retorna os dados na struct Produto */
Produto recuperar_registro(int rrn);

/* Realiza os scanfs na struct Produto */
void ler_entrada(char* registro, Produto *novo);

// ================= ROTINAS DE MANIPULAÇÃO DE ÍNDICES ======================

void inserir_primary(Produto p, Ip *indice, int nregistros);

void inserir_icategory(Produto p, Ir *icategory, int *ncat);

// Insere o produto na lista ligada de uma das categorias existentes
void inserir_produto_cat_ll(char* pk, Ir* categoria);

void inserir_secundarios(Produto novo, Is* iproduct, Is* ibrand, Ir *icategory, Isf* iprice, int nregistros, int* ncat);

// ========================= ROTINAS DE EXIBIÇÃO ============================

// Rotina geral de busca
void buscar(Ip *iprimary, Is *iproduct, Is *ibrand, Ir *icategory, Isf *iprice, int nregistros, int ncat);

// Rotina geral para listagem
void listar(Ip *iprimary, Is *iproduct, Is *ibrand, Ir *icategory, Isf *iprice, int nregistros, int ncat);


/* Rotina para impressao de indice secundario */
void imprimirSecundario(Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int nregistros, int ncat);

/* Exibe o Produto */
int exibir_registro(int rrn, char com_desconto);

// =========================== ROTINAS AUXILIARES ==========================

// Recebe uma struct produto e gera o campo pk para ela
void gerarChave(Produto *novo);

// Retorna 1 caso a chave informada não esteja no índice, 0 caso contrário
int isUniquePK(char *pk, Ip* iprimary, int nregistros);

// Recebe uma pk e busca o rrn no índice primário
int getrrn(char *pk, Ip *iprimary, int nregistros);

// ========================= ROTINAS DE COMPARAÇÃO ==========================

// qsort
int cmp_ip(const void* ip1, const void* ip2);
int cmp_is(const void* is1, const void* is2);
int cmp_isf(const void* isf1, const void* isf2);
int cmp_ir(const void* ir1, const void* ir2);

// Busca binária
int cmp_str_ip(const void* chave, const void* elemento);
int cmp_str_is(const void* chave, const void* elemento);
int cmp_str_ir(const void* ir1, const void* ir2);


/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main(){
  /* Arquivo */
	int carregarArquivo = 0, nregistros = 0, ncat = 0;
	char registro[TAM_REGISTRO];
	Produto novo;


	scanf("%d%*c", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	/* Índice primário */
	Ip *iprimary = (Ip *) malloc (MAX_REGISTROS * sizeof(Ip));
  	if (!iprimary) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	criar_iprimary(iprimary, &nregistros);

	/*Alocar e criar índices secundários*/
	// iproduct
	Is *iproduct = malloc(MAX_REGISTROS * sizeof(Is));
	Is *ibrand = malloc(MAX_REGISTROS * sizeof(Is));
	Isf *iprice = malloc(MAX_REGISTROS * sizeof(Isf));
	Ir *icategory = malloc(MAX_CATEGORIAS * sizeof(Ir));
	criar_secundarios(iproduct, ibrand, icategory, iprice, nregistros, &ncat);
	
	/* Execução do programa */
	int opcao = 0;
	while(1)
	{
		scanf("%d%*c", &opcao);
		switch(opcao)
		{
			case 1:
				ler_entrada(registro, &novo);
				if (!isUniquePK(novo.pk, iprimary, nregistros)){
					printf(ERRO_PK_REPETIDA, novo.pk);
					break;
				}
				strcat(ARQUIVO, registro);
				nregistros++;
				inserir_primary(novo, iprimary, nregistros);
				inserir_secundarios(novo, iproduct, ibrand, icategory, iprice, nregistros, &ncat);
			break;
			case 2:
				/*alterar desconto*/
				printf(INICIO_ALTERACAO);
				/*
				if(alterar([args]))
					printf(SUCESSO);
				else
					printf(FALHA);
				*/
			break;
			case 3:
				/*excluir produto*/
				printf(INICIO_EXCLUSAO);
				/*
				if(remover([args]))
					printf(SUCESSO);
				else
					printf(FALHA);
				*/
			break;
			case 4:
				/*busca*/
				printf(INICIO_BUSCA );
				buscar(iprimary, iproduct, ibrand, icategory, iprice, nregistros, ncat);
			break;
			case 5:
				/*listagens*/
				printf(INICIO_LISTAGEM);
				listar(iprimary, iproduct, ibrand, icategory, iprice, nregistros, ncat);
			break;
			case 6:
				/*libera espaço*/
			break;
			case 7:
				/*imprime o arquivo de dados*/
				printf(INICIO_ARQUIVO);
				printf("%s\n", ARQUIVO);
			break;
			case 8:
				/*imprime os índices secundários*/
				imprimirSecundario(iproduct, ibrand, icategory, iprice, nregistros, ncat);
			break;
			case 9:
	      		/*Liberar memória e finalizar o programa */
				return 0;
			break;
			default:
				printf(OPCAO_INVALIDA);
			break;
		}
	}
	return 0;
}

int carregar_arquivo()
{
	scanf("%[^\n]\n", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
}

void criar_iprimary(Ip *indice_primario, int* nregistros){
	int i;
	int nreg = *nregistros;
	Produto p;
	
	for(i=0;i<nreg;i++){
		p = recuperar_registro(i); // Recupera dados do registro de rrn i e os copia pra variável p
		strcpy(indice_primario[i].pk, p.pk); // Copia PK pro índice
		indice_primario[i].rrn = i; // Salva rrn do registro no índice
	}

	qsort(indice_primario, nreg, sizeof(Ip), cmp_ip);
}

void criar_secundarios(Is *iproduct, Is *ibrand, Ir *icategory, Isf *iprice, int nregistros, int *ncat){
	int i;
	Produto p;
	int desconto;
	float preco;
	
	for(i=0;i<nregistros;i++){
		p = recuperar_registro(i); // Recupera dados do registro de rrn i e os copia pra variável p

		// iproduct
		strcpy(iproduct[i].pk, p.pk); // Copia PK pro índice
		strcpy(iproduct[i].string, p.nome); // Copia PK pro índice

		// ibrand
		strcpy(ibrand[i].pk, p.pk); // Copia PK pro índice
		strcpy(ibrand[i].string, p.marca); // Copia PK pro índice

		// iprice
		strcpy(iprice[i].pk, p.pk); // Copia 
		sscanf(p.desconto,"%d",&desconto);
		sscanf(p.preco,"%f",&preco);
		preco = (preco *  (100-desconto))/100.0;
		preco = preco * 100;
		iprice[i].price = ((int) preco)/ (float) 100 ;

		// icategory (ordena automáticamente)
		inserir_icategory(p, icategory, ncat);
	}

	qsort(iproduct, nregistros, sizeof(Is), cmp_is);
	qsort(ibrand, nregistros, sizeof(Is), cmp_is);
	qsort(iprice, nregistros, sizeof(Isf), cmp_isf);
}

/* Recupera do arquivo o registro com o rrn
 * informado e retorna os dados na struct Produto */
Produto recuperar_registro(int rrn)
{
	char temp[TAM_REGISTRO+1], *p;
	strncpy(temp, ARQUIVO + ((rrn)*TAM_REGISTRO), TAM_REGISTRO);
	temp[TAM_REGISTRO] = '\0';
	Produto j;
	p = strtok(temp,"@");
	strcpy(j.nome,p);
	p = strtok(NULL,"@");
	strcpy(j.marca,p);
	p = strtok(NULL,"@");
	strcpy(j.data,p);
	p = strtok(NULL,"@");
	strcpy(j.ano,p);
	p = strtok(NULL,"@");
	strcpy(j.preco,p);
	p = strtok(NULL,"@");
	strcpy(j.desconto,p);
	p = strtok(NULL,"@");
	strcpy(j.categoria,p);
	gerarChave(&j);
	return j;
}

void inserir_primary(Produto p, Ip *indice, int nregistros){
	strcpy(indice[nregistros-1].pk, p.pk); // Acrescenta nova chave primária
	indice[nregistros-1].rrn = nregistros - 1; // no final do vetor

	qsort(indice, nregistros, sizeof(Ip), cmp_ip); // reordena
}

void inserir_icategory(Produto p, Ir *icategory, int *ncat){
	char temp[TAM_CATEGORIA];
	char *tok;
	Ir *cat;
	int nAnteriorCat = *ncat;

	strcpy(temp, p.categoria);

	tok = strtok(temp, "|");
	while (tok != NULL){
		cat = bsearch(tok, icategory, *ncat, sizeof(Ir), cmp_str_ir);
		// Se achou categoria com o nome tok
		if (cat != NULL){
			// insere a chave do produto na lista ligada de cat
			inserir_produto_cat_ll(p.pk, cat);
		} else {
			strcpy(icategory[*ncat].cat, tok); // Insere categoria no vetor
			icategory[*ncat].lista = NULL; // instancia lsita
			inserir_produto_cat_ll(p.pk, &icategory[*ncat]); // insere pk do produto na lista
			(*ncat)++; // incrementa o número de categorias
		}
		tok = strtok(NULL, "|");
	}

	// Se aumentou o número de categorias
	if (*ncat > nAnteriorCat){
		// Reordena o vetor
		qsort(icategory, *ncat, sizeof(Ir), cmp_ir);
	}
}

// Insere o produto na lista ligada de uma das categorias existentes
void inserir_produto_cat_ll(char* pk, Ir* categoria){
	ll* novo = malloc(sizeof(ll));
	strcpy(novo->pk, pk);

	// primeiro da lista
	ll* aux = categoria->lista;
	ll* prox;

	// Se a lista estiver vazia ou se o primeiro elemento for maior que o elem a ser inserido
	// insere no início da lista
	if (aux == NULL || strcmp(aux->pk, novo->pk) > 0){
		novo->prox = aux;
		categoria->lista = novo;
		return;
	}

	do {
		prox = aux->prox;
		if (prox == NULL || strcmp(prox->pk, novo->pk) > 0){
			aux->prox = novo;
			novo->prox = prox;
			return;
		}
		aux = prox;
	} while (aux != NULL);
}

void inserir_secundarios(Produto novo, Is* iproduct, Is* ibrand, Ir *icategory, Isf* iprice, int nregistros, int* ncat){
	int desconto;
	float preco;

	// iproduct
	strcpy(iproduct[nregistros-1].pk, novo.pk); // Copia PK pro índice
	strcpy(iproduct[nregistros-1].string, novo.nome); // Copia PK pro índice
	// ibrand
	strcpy(ibrand[nregistros-1].pk, novo.pk); // Copia PK pro índice
	strcpy(ibrand[nregistros-1].string, novo.marca); // Copia PK pro índice
	// iprice
	strcpy(iprice[nregistros-1].pk, novo.pk); // Copia PK pro índice
	sscanf(novo.desconto,"%d",&desconto);
	sscanf(novo.preco,"%f",&preco);
	preco = (preco *  (100-desconto))/100.0;
	preco = preco * 100;
	iprice[nregistros-1].price = ((int) preco)/ (float) 100 ;

	// icategory (ordena automáticamente)
	inserir_icategory(novo, icategory, ncat);

	qsort(iproduct, nregistros, sizeof(Is), cmp_is);
	qsort(ibrand, nregistros, sizeof(Is), cmp_is);
	qsort(iprice, nregistros, sizeof(Isf), cmp_isf);
}

// ========================= ROTINAS DE EXIBIÇÃO ============================
/* Exibe o Produto */
int exibir_registro(int rrn, char com_desconto)
{
	if(rrn<0)
		return 0;
	float preco;
	int desconto;
	Produto j = recuperar_registro(rrn);
  	char *cat, categorias[TAM_CATEGORIA];
	printf("%s\n", j.pk);
	printf("%s\n", j.nome);
	printf("%s\n", j.marca);
	printf("%s\n", j.data);
	if(!com_desconto)
	{
		printf("%s\n", j.preco);
		printf("%s\n", j.desconto);
	}
	else
	{
		sscanf(j.desconto,"%d",&desconto);
		sscanf(j.preco,"%f",&preco);
		preco = preco *  (100-desconto);
		preco = ((int) preco)/ (float) 100 ;
		printf("%07.2f\n",  preco);

	}
	strcpy(categorias, j.categoria);

	cat = strtok (categorias, "|");

	while(cat != NULL){
		printf("%s", cat);
		cat = strtok (NULL, "|");
		if(cat != NULL){
			printf(", ");
		}
	}

	printf("\n");

	return 1;
}

// Rotina geral de busca
void buscar(Ip *iprimary, Is *iproduct, Is *ibrand, Ir *icategory, Isf *iprice, int nregistros, int ncat){
	int opBusca = 0;
	int rrn;
	int i;
	Ip *paux;
	Is *saux;
	Is *sprev;
	Ir *raux;
	ll *laux;

	char temp[TAM_NOME];
	char catNome[TAM_CATEGORIA];

	scanf("%d%*c", &opBusca);
	switch (opBusca) {
		case 1: // Por código
			scanf("%[^\n]%*c", temp); // lê PK do teclado
			paux = bsearch(temp, iprimary, nregistros, sizeof(Ip), cmp_str_ip);
			
			if (paux){
				exibir_registro(paux->rrn, 0);
			} else {
				printf(REGISTRO_N_ENCONTRADO);
			}
		break;

		case 2: // Por nome 
			scanf("%[^\n]%*c", temp); // lê nome do teclado
			saux = bsearch(temp, iproduct, nregistros, sizeof(Is), cmp_str_is);
			if (!saux){
				printf(REGISTRO_N_ENCONTRADO);
				break;
			}

			// Caso a bb não tenha retornado o primeiro elemento com o nome temp
			// "rebobina" o array até achar o primeiro elemento de nome temp
			sprev = saux;
			sprev--;
			while (sprev && strcmp(temp, sprev->string) == 0){
				saux = sprev;
				sprev--;
			}

			// Varre indice exibindo registros correspondentes a chave de busca
			do {
				rrn = getrrn(saux->pk, iprimary, nregistros);
				exibir_registro(rrn, 0);
				saux++;
				if (saux){
					i = strcmp(temp, saux->string);
				} else {
					i = -1;
				}
				if (i == 0)
					printf("\n");
			} while (i == 0);
		break;

		case 3: // Por marca e categoria
			scanf("%[^\n]%*c", temp); // lê marca do teclado
			scanf("%[^\n]%*c", catNome); // lê categoria do teclado
			saux = bsearch(temp, ibrand, nregistros, sizeof(Is), cmp_str_is);
			if (!saux){
				printf(REGISTRO_N_ENCONTRADO);
				break;
			}

			// Caso a bb não tenha retornado o primeiro elemento com a marca temp
			// "rebobina" o array até achar o primeiro elemento de marca temp
			sprev = saux;
			sprev--;
			while (saux && strcmp(temp, sprev->string) == 0){
				saux = sprev;
				sprev--;
			}

			int numRetornos = 0;
			// Varre indice processando registros da marca informada
			do {
				raux = bsearch(catNome, icategory, ncat, sizeof(Ir), cmp_str_ir);
				if (raux) {
					laux = raux->lista;

					while (laux != NULL){
						if (strcmp(saux->pk, laux->pk) == 0){
							if (numRetornos > 0)
								printf("\n");
							numRetornos++;
							rrn = getrrn(laux->pk, iprimary, nregistros);
							exibir_registro(rrn, 0);
						}
						laux = laux->prox;
					}
				}
				saux++;
				if (saux){
					i = strcmp(temp, saux->string);
				} else {
					i = -1;
				}
				if (i == 0)
					printf("\n");					
			} while (i == 0);

			if (numRetornos == 0)
				printf(REGISTRO_N_ENCONTRADO);
		break;
	}
}

// Rotina geral para listagem
void listar(Ip *iprimary, Is *iproduct, Is *ibrand, Ir *icategory, Isf *iprice, int nregistros, int ncat){
	int opList = 0, i, rrn;
	char catNome[TAM_CATEGORIA];

	scanf("%d%*c", &opList);
	if (!nregistros){
		printf(REGISTRO_N_ENCONTRADO);
		return;
	}
	switch (opList) {
		case 1:
			for (int i = 0; i < nregistros; i++){
				if (i > 0)
					printf("\n");
				exibir_registro(iprimary[i].rrn, 0);
			}
		break;

		case 2:
			scanf("%[^\n]%*c", catNome); // lê categoria a ser listada
			Ir *cat = bsearch(catNome, icategory, ncat, sizeof(Ir), cmp_str_ir);
			ll *aux = cat->lista;
			
			while (aux != NULL){
				rrn = getrrn(aux->pk, iprimary, nregistros);
				exibir_registro(rrn, 0);

				aux = aux->prox;
				if (aux)
					printf("\n");
			}
		break;

		case 3:
			for (int i = 0; i < nregistros; i++){
				rrn = getrrn(ibrand[i].pk, iprimary, nregistros);
				if (i > 0)
					printf("\n");
				exibir_registro(rrn, 0);
			}
		break;

		case 4:
			for (int i = 0; i < nregistros; i++){
				rrn = getrrn(iprice[i].pk, iprimary, nregistros);
				if (i > 0)
					printf("\n");
				exibir_registro(rrn, 1);
			}
		break;
	}
}

/* Imprimir indices secundarios */
void imprimirSecundario(Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int nregistros, int ncat){
	int opPrint = 0;
	ll *aux;
	printf(INICIO_ARQUIVO_SECUNDARIO);
	scanf("%d", &opPrint);
	if(!nregistros)
		printf(ARQUIVO_VAZIO);
	switch (opPrint) {
		case 1:
			for(int i = 0; i < nregistros; i++){
				printf("%s %s\n",iproduct[i].pk, iproduct[i].string);
			}
		break;
		case 2:
			for(int i = 0; i < nregistros; i++){
				printf("%s %s\n",ibrand[i].pk, ibrand[i].string);
			}
		break;
		case 3:
			for(int i = 0; i < ncat; i++){
				printf("%s", icategory[i].cat);
				aux =  icategory[i].lista;
				while(aux != NULL){
					printf(" %s", aux->pk);
					aux = aux->prox;
				}
				printf("\n");
			}
		break;

		case 4:
		for(int i = 0; i < nregistros; i++){
			printf("%s %.2f\n",iprice[i].pk, iprice[i].price);
		}
		break;
	}
}

/* Realiza os scanfs na struct Produto */
void ler_entrada(char *registro, Produto *novo){
	int i, tamanho_registro;

	// nome
	scanf("%[^\n]%*c", novo->nome);

	// marca
	scanf("%[^\n]%*c", novo->marca);

	// data
	scanf("%[^\n]%*c", novo->data);
	
	// ano
	scanf("%[^\n]%*c", novo->ano);

	// preco
	scanf("%[^\n]%*c", novo->preco);

	// desconto
	scanf("%[^\n]%*c", novo->desconto);

	// categoria
	scanf("%[^\n]%*c", novo->categoria);

	// pk
	gerarChave(novo);

	// Criação do registro
	strcpy(registro, novo->nome);
	strcat(registro, "@");
	strcat(registro, novo-> marca);
	strcat(registro, "@");
	strcat(registro, novo-> data);
	strcat(registro, "@");
	strcat(registro, novo-> ano);
	strcat(registro, "@");
	strcat(registro, novo-> preco);
	strcat(registro, "@");
	strcat(registro, novo-> desconto);
	strcat(registro, "@");
	strcat(registro, novo-> categoria);
	strcat(registro, "@");

	tamanho_registro = strlen(registro);

	for (i = 0; i < (TAM_REGISTRO - tamanho_registro); i++){
		strcat(registro, "#");
	}
}

// =========================== ROTINAS AUXILIARES ==========================

// Recebe uma struct produto e gera o campo pk para ela
void gerarChave(Produto *novo){
	novo->pk[0] = novo->nome[0];	//G
	novo->pk[1] = novo->nome[1];	//E
	novo->pk[2] = novo->marca[0];	//M
	novo->pk[3] = novo->marca[1];	//S
	novo->pk[4] = novo->data[0];	//1
	novo->pk[5] = novo->data[1];	//1
	novo->pk[6] = novo->data[3];	//0
	novo->pk[7] = novo->data[4];	//9
	novo->pk[8] = novo->ano[0];		//1
	novo->pk[9] = novo->ano[1];		//7
	novo->pk[10] = '\0';			//\0
}

// Retorna 1 caso a chave informada não esteja no índice, 0 caso contrário
int isUniquePK(char *pk, Ip* iprimary, int nregistros){
	Ip* elem = bsearch(pk, iprimary, nregistros, sizeof(Ip), cmp_str_ip);

	if (!elem)
		return 1;
	else
		return 0;
}

// Recebe uma pk e busca o rrn no índice primário
int getrrn(char *pk, Ip *iprimary, int nregistros){
	Ip *reg = bsearch(pk, iprimary, nregistros, sizeof(Ip), cmp_str_ip);
	if(!reg)
		return -1;
	else
		return reg->rrn;
}

// ========================= ROTINAS DE COMPARAÇÃO ==========================

int cmp_ip(const void* ip1, const void* ip2){
	Ip *a = (Ip *)ip1;
  	Ip *b = (Ip *)ip2;

	return strcmp(a->pk, b->pk);
}

int cmp_is(const void* is1, const void* is2){
	int ret;
	Is *a = (Is *)is1;
  	Is *b = (Is *)is2;

	ret = strcmp(a->string, b->string);
	if (ret == 0)
		return strcmp(a->pk, b->pk);
	else
		return ret;
}

int cmp_isf(const void* isf1, const void* isf2){
	int ret;
	Isf *a = (Isf *)isf1;
  	Isf *b = (Isf *)isf2;

	if (a->price == b->price){
		return strcmp(a->pk, b->pk);
	}

	if (a->price > b->price)
		return 1;
	else 
		return -1;
}

int cmp_ir(const void* ir1, const void* ir2){
	Ir *a = (Ir *)ir1;
  	Ir *b = (Ir *)ir2;

	return strcmp(a->cat, b->cat);
}

int cmp_str_ip(const void* chave, const void* elemento){
	char *key = (char *)chave;
	Ip *indice = (Ip *)elemento;

	return strcmp(key, indice->pk);
}

int cmp_str_is(const void* chave, const void* elemento){
	char *key = (char *)chave;
	Is *indice = (Is *)elemento;

	return strcmp(key, indice->string);
}

int cmp_str_ir(const void* chave, const void* elemento){
	char *key = (char *)chave;
	Ir *indice = (Ir *)elemento;

	return strcmp(key, indice->cat);
}