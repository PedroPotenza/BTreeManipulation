/*
    Alunos:
        Gabriel Tetzlaf Mansano - 201150956
        Pedro Henrique Potenza Fernandes - 201151219
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
/*------------------------------------ Defines --------------------------------------*/
#define true 1
#define false 0
#define NIL -1
#define MAXKEYS 3
#define NOKEY '@'
#define PAGESIZE 47
#define REGISTERSIZE 156

typedef struct s_Key {
    char ClientId[3];
    char MovieId[3];
} KEY;

typedef struct s_KeyPage {
    char Id[5];
    int rrn;
} KEYPAGE;

typedef struct s_Register {
    KEY Id;
    char ClientName[50];
    char MovieName[50];
    char Genre[50];
} REGISTER;

typedef struct {
    int keyCount;
    KEYPAGE keys[3];
    int childs[4];
} PAGE;

FILE * readPositions();
void savePosition();

void insertRegister(REGISTER newRegister);

int insertRegisterIndex(int rrn, KEYPAGE key, int* promo_right_child, KEYPAGE* promo_key, FILE* file);
int createRoot(KEYPAGE key, int left, int right, FILE* file);
int getPage(FILE* file);
void initPag(PAGE* page);
void writeIndex(int rrn, PAGE* page, FILE* file);
void readPage (int rrn, PAGE* page, FILE* file);
int searchNode(KEYPAGE key, PAGE* page, int *pos);
void insertInPage (KEYPAGE key, int rightChild, PAGE* page);
void split (KEYPAGE key, int rightChild, PAGE* oldPage, KEYPAGE* promo_key, int* promo_right_child, PAGE* newPage, FILE* file);

void PrintTreeInOrder(int rrn, FILE* file);
void PrintRegisterByIndex(int rrn, int position, int rrnRegister,FILE* file);

int findRegister(int rrn, KEYPAGE keyToFind, FILE* file);

/*------------------------------------ Globals --------------------------------------*/
int inseridos = 0;
int buscas = 0;

/*------------------------------------ Util --------------------------------------*/
FILE * fileOpenRead(char * filename) {
	FILE *file = fopen(filename, "rb");
	
	if(file == NULL) {
		printf("O arquivo %s nao pode ser aberto.", filename);
		exit(1);
	}
	
	return file;	
}

FILE * readPositions() {
	char * filename = "position.bin";
	int ZERO = 0;
	
	if(access(filename, F_OK ) == 0)
		return fopen(filename, "r+b");
		
	FILE * file = fopen(filename, "w+b");

	fwrite(&ZERO, sizeof(int), 1, file);
	fwrite(&ZERO, sizeof(int), 1, file);

	return file;
}

void savePosition() {

    char * filename = "position.bin";
	
	FILE * file = fopen(filename, "w+b");

	fwrite(&inseridos, sizeof(int), 1, file);
	fwrite(&buscas, sizeof(int), 1, file);

	fclose(file);
}

/*------------------------------------ Print --------------------------------------*/
void PrintRegisterByIndex(int rrn, int position, int rrnRegister,FILE* file) {

    printf("\n=============== Pagina %d === Posicao %d ===============\n", rrn, position);

    int adress = (rrnRegister * REGISTERSIZE);

    FILE* dataFile = fopen("registers.bin", "r+b");
    fseek(dataFile, adress, SEEK_SET);

    REGISTER registerLocal;
    fread(&registerLocal.Id.ClientId, 3, sizeof(char), dataFile);
    fread(&registerLocal.Id.MovieId, 3, sizeof(char), dataFile);
    fread(&registerLocal.ClientName, 50, sizeof(char), dataFile);
    fread(&registerLocal.MovieName, 50, sizeof(char), dataFile);
    fread(&registerLocal.Genre, 50, sizeof(char), dataFile);
    
    printf("Id Cliente: %s\n", registerLocal.Id.ClientId);
    printf("Id Filme: %s\n", registerLocal.Id.MovieId);
    printf("Nome Cliente: %s\n", registerLocal.ClientName);
    printf("Nome Filme: %s\n", registerLocal.MovieName);
    printf("Genero: %s\n\n", registerLocal.Genre);

}

void PrintTreeInOrder(int rrn, FILE* file) { 

    PAGE page;

    readPage(rrn, &page, file);

    if(page.childs[0] == -1) {
        for (int i = 0; i < page.keyCount; i++)
        {
            PrintRegisterByIndex(rrn, i, page.keys[i].rrn, file);
        }
        return;
    }
    else 
        PrintTreeInOrder(page.childs[0], file);

    for (int i = 0; i < page.keyCount; i++)
    {
        PrintRegisterByIndex(rrn, i, page.keys[i].rrn,file);
        PrintTreeInOrder(page.childs[i+1], file);
    }

}

/*------------------------------------ Insert --------------------------------------*/
/**
 * @brief Insere um registro no final do arquivo "registers.bin"
 * 
 * @param newRegister registro a ser inserido 
 */
void insertRegister(REGISTER newRegister) {

    FILE* dataFile = fopen("registers.bin", "r+b");
    fseek(dataFile, 0, SEEK_END);

    // printf("Id Cliente: %s\n", newRegister.Id.ClientId);
    // printf("Id Filme: %s\n", newRegister.Id.MovieId);
    // printf("Nome Cliente: %s\n", newRegister.ClientName);
    // printf("Nome Filme: %s\n", newRegister.MovieName);
    // printf("Genero: %s\n", newRegister.Genre);

    fwrite(&newRegister.Id.ClientId, 3, sizeof(char), dataFile);
    fwrite(&newRegister.Id.MovieId, 3, sizeof(char), dataFile);
    fwrite(&newRegister.ClientName, 50, sizeof(char), dataFile);
    fwrite(&newRegister.MovieName, 50, sizeof(char), dataFile);
    fwrite(&newRegister.Genre, 50, sizeof(char), dataFile);

    fclose(dataFile);

    char key[5];

    strcpy(key, newRegister.Id.ClientId);
    strcat(key, newRegister.Id.MovieId);

    printf("\nChave %s inserida com sucesso!", key);

}

/**
 * @brief Descobre o rrn da nova pagina
 * 
 * @return int o rrn da nova pagina
 */
int getPage(FILE* file) {
    int adress;
    fseek(file, 0, SEEK_END); //arquivo do index
    adress = ftell(file);
    adress -= 4;
    if(adress < 0)
        return 0;
    return adress/PAGESIZE;
}

int getRegisterRrn() {

    FILE* dataFile = fopen("registers.bin", "r+b");
    fseek(dataFile, 0, SEEK_END);
    int adress = ftell(dataFile);
    return adress/REGISTERSIZE;
    fclose(dataFile);
}

/**
 * @brief Cria a pagina
 * 
 * @param page informacoes da pagina
 */
void initPage(PAGE* page)
{ 
    for (int i = 0; i < MAXKEYS; i++)
    {
        strcpy(page->keys[i].Id, "@@@@");
        page->keys[i].rrn = NIL;

        page->childs[i] = NIL;
    }
    page->childs[MAXKEYS] = NIL;
    page->keyCount = 0;
}

/**
 * @brief escreve a pagina no arquivo index
 * 
 * @param rrn 
 * @param page 
 */
void writeIndex(int rrn, PAGE* page, FILE* file) {

    int adress = (rrn * PAGESIZE) + sizeof(int);
    fseek(file, adress, SEEK_SET);
    // fwrite(page, sizeof(PAGE), 1, file);

    fwrite(&page->keyCount, 1, sizeof(int), file);

    for (int i = 0; i < MAXKEYS; i++)
    {
        fwrite(&page->keys[i].Id, 5, sizeof(char), file);
        fwrite(&page->keys[i].rrn, 1, sizeof(int), file);
    }

    for (int i = 0; i < MAXKEYS + 1; i++)
    {
        fwrite(&page->childs[i], 1, sizeof(int), file);
    }
    
}


int createRoot(KEYPAGE key, int left, int right, FILE* file) { 
    PAGE page; 
    int rrn;

    rrn = getPage(file);
    initPage(&page);
    // printf("Criou a pagina %d e inicializou ela com tudo nulo.\n", rrn);

    page.keys[0] = key;
    page.childs[0] = left;
    page.childs[1] = right;
    page.keyCount = 1;
    // printf("Adicionou o registro na pagina %d.\n", rrn);

    writeIndex(rrn, &page, file);
    // printf("Gravou a pagina no  bTreeIndex.bin\n");

    rewind(file);
    fwrite(&rrn, 1, sizeof(int), file);
    // printf("Reescrevendo o header para %d.\n", rrn);

    return rrn;

}

/**
 * @brief Se desloca dentro do arquivo bTreeIndex.bin para a posicao do rrn, le a pagina e carrega ela na variavel page passada por referencia
 * 
 * @param rrn pagina a ser lida 
 * @param page a pagina na memoria a ser carregada
 */
void readPage (int rrn, PAGE* page, FILE* file) { 
    
    int adress = (rrn * PAGESIZE) + sizeof(int);
    fseek(file, adress, SEEK_SET);
    // fread(page, 1, sizeof(PAGE), file);

    fread(&page->keyCount, 1, sizeof(int), file);

    for (int i = 0; i < MAXKEYS; i++)
    {
        fread(&page->keys[i].Id, 5, sizeof(char), file);
        fread(&page->keys[i].rrn, 1, sizeof(int), file);
    }

    for (int i = 0; i < MAXKEYS + 1; i++)
    {
        fread(&page->childs[i], 1, sizeof(int), file);
        
    }
    
}

/**
 * @brief Tenta localizar a chave dentro da pagina e salva a posicao que ela esta ou que deveria estar  
 * 
 * @param key chave a ser localizada
 * @param page pagina a ser analisada
 * @param pos a posicao que a chave esta ou deveria estar
 * @return int caso a chave exista ja retorna true, caso nao exista retorna false
 */
int searchNode(KEYPAGE key, PAGE* page, int *pos) {
       
    int i;

    //se a chave procurada for maior que aquela chave da pagina
    for (i = 0; i < page->keyCount && strcmp(key.Id,page->keys[i].Id) > 0; i++);
    
    *pos = i;

    if(*pos < page->keyCount && strcmp(key.Id,page->keys[*pos].Id) == 0){
        return true;
    }
    else 
        return false;
}

void insertInPage (KEYPAGE key, int rightChild, PAGE* page) {

    //se a chave procurada for menor que a chave daquela posicao da pagina
    int i;

    for (i = page->keyCount; strcmp(key.Id,page->keys[i-1].Id) < 0 && i > 0; i--) 
    {
        page->keys[i] = page->keys[i-1];
        page->childs[i+1] = page->childs[i];
    }
    
    page->keyCount++;
    page->keys[i]= key;
    page->childs[i+1] = rightChild;
  
}

/**
 * @brief Divide a pagina e sobe o da esquerda
 * 
 * @param key chave a ser inserida 
 * @param rightChild filho da direita dela 
 * @param oldPage pagina antiga (q tem que ser dividida)
 * @param promo_key chave a ser promovida
 * @param promo_righ_child o filho da chave a ser promovida
 * @param newPage a nova pagina feita com metade da pagina antiga
 */
void split (KEYPAGE key, int rightChild, PAGE* oldPage, KEYPAGE* promo_key, int* promo_right_child, PAGE* newPage, FILE* file) {
    
    int i; 
    //cria 2 vetores auxiliares, um com as chaves e o outro com os filhos dessas chaves
    KEYPAGE workKeys[MAXKEYS + 1];
    int workChild[MAXKEYS + 2];

    //salvo as informacoes da pagina antiga nessa variavel local
    for (i = 0; i < 3; i++)
    {
        workKeys[i] = oldPage->keys[i];

        workChild[i] = oldPage->childs[i];
    }
    
    //nao esqueco de salvar o ultimo filho (pq tem uma posicao a mais)
    workChild[i] = oldPage->childs[i];

    //insiro a chave na pagina auxiliar ja de forma ordenada 
    for (i = MAXKEYS; strcmp(key.Id,workKeys[i-1].Id) < 0 && i > 0; i--) {

        
        workKeys[i] = workKeys[i-1];
        workChild[i+1] = workChild[i];
    }
    workKeys[i] = key;
    workChild[i+1] = rightChild;

    //salva o rrn da pagina nova a ser criada
    *promo_right_child = getPage(file);
    //cria a pagina
    initPage(newPage);

    //preencher a pagina velha e a pagina nova com as informacoes corretas
    oldPage->keys[0] = workKeys[0];
    oldPage->childs[0] = workChild[0];

    for (int i = 1; i <= 2; i++)
    {
        strcpy(oldPage->keys[i].Id, "@@@@");
        oldPage->keys[i].rrn = NIL;
        oldPage->childs[i] = NIL;
    }
    oldPage->childs[3] = NIL;
    oldPage->childs[1] = workChild[1];
    oldPage->keyCount = 1;

    printf("Chave %s promovida!\n", workKeys[1].Id);
        *promo_key = workKeys[1];

    newPage->keys[0] = workKeys[2];
    newPage->childs[0] = workChild[2];

    newPage->keys[1] = workKeys[3];
    newPage->childs[1] = workChild[3];

    newPage->childs[2] = workChild[4];

    newPage->keyCount = 2;
    
}

/**
 * @brief Funcao reutilizada dentro da arvore para inserir uma chave 
 * 
 * @param RRN root ou rrn da pagina pai
 * @param key chave a ser inserida
 * @param promo_right_child caso seja necessario promover uma chave, guarda o "ponteiro" da direita (guarda o filho direito)
 * @param promo_key caso seja necessario promover, guarda o valor da chave a ser promovida
 * @return int retorna 0 caso a chave seja duplicada, retorna true se a chave for promovida (ou chegou na folha) 
 */
int insertRegisterIndex(int rrn, KEYPAGE key, int* promo_right_child, KEYPAGE* promo_key, FILE* file){

    PAGE page, newpage;
    int found, promoted; 
    int pos, rrnPromotedBelow;
    KEYPAGE keyPromotedBelow;

    // printf("insertRegisterIndex com rrn \"%d\"\n", rrn);

    // se for uma folha
    if(rrn == -1) {

        // printf("entrou no if do rrn -1\n");

        *promo_key = key;
        *promo_right_child = NIL;
        return(1);

    }

    //le a pagina daquele rrn, colocando ela na memória (carregando a variavel page)
    readPage(rrn, &page, file);
    
    //tenta achar a chave dentro da pagina
    found = searchNode(key, &page, &pos); 
    if(found) { 
        return 3;
    }

    promoted = insertRegisterIndex(page.childs[pos], key, &rrnPromotedBelow, &keyPromotedBelow, file);

    //se a chave foi inserida na pagina, ela nao foi promovida, logo para a recursao
    if(!promoted) {
        return false;
    }

    if(promoted == 3) {
        return 3;
    }

    //se tem espaco naquela pagina, insere la 
    if(page.keyCount < MAXKEYS) {

        // printf("tem espaco na pagina \"%d\"\n", rrn);
        
        insertInPage(keyPromotedBelow, rrnPromotedBelow, &page);
        writeIndex(rrn, &page, file);
        return false;
    } else { 
        //se nao tem espaco na pagina, divide ela 
        printf("Divisao de no.\n");
        split(keyPromotedBelow, rrnPromotedBelow, &page, promo_key, promo_right_child, &newpage, file);
        // printf("AAAAAAA RRN: %d\n", rrn);
        writeIndex(rrn, &page, file);
        writeIndex(*promo_right_child, &newpage, file);
        return true;
    }

}

/*------------------------------------ Find --------------------------------------*/
int findRegister(int rrn, KEYPAGE keyToFind, FILE* file) {

    PAGE page;
    int found = 0, tryAgain, pos; 

    if(rrn == -1) 
        return false;
    //le a pagina daquele rrn, colocando ela na memória (carregando a variavel page)
    readPage(rrn, &page, file);
    
    //tenta achar a chave dentro da pagina
    found = searchNode(keyToFind, &page, &pos); 
    if(found) { 

        PrintRegisterByIndex(rrn, pos, page.keys[pos].rrn,file);
        return true;
    }
    
    tryAgain = findRegister(page.childs[pos], keyToFind, file);

    return tryAgain;

}

/*------------------------------------ Main --------------------------------------*/
int main(int argc, char const *argv[])
{
    
    //carrega em memoria principal o vetor insere
    FILE* file = fileOpenRead("insere.bin");

    REGISTER* insertData;
    int insertSize = 15;

    insertData = (REGISTER*) malloc(insertSize * sizeof(REGISTER));

    fread(insertData, sizeof(REGISTER), insertSize, file);
    fclose(file);

    //carrega em memoria principal o vetor busca
    file = fileOpenRead("busca.bin");

    KEY* buscaData;
    int buscaSize = 15;

    buscaData = (KEY*) malloc(buscaSize * sizeof(KEY));

    fread(buscaData, sizeof(KEY), buscaSize, file);
    fclose(file);

    FILE* dataFile;

    if(access("registers.bin", F_OK ) == 0 ) {
	    dataFile = fopen("registers.bin", "r+b");
    } else {
       dataFile = fopen("registers.bin", "w+b");
    }

    fclose(dataFile);

    file = readPositions();
        fread(&inseridos, sizeof(int), 1, file);
        fread(&buscas, sizeof(int), 1, file);
    fclose(file);

    printf("\n---------- Contador Inicial ----------\n");
    printf("Registros inseridos: %d\n", inseridos);
    printf("Buscas efetuadas: %d\n\n", buscas);

    printf("--------- Menu ---------\n");
    printf(" (1) -> Inserir Registro\n");
    printf(" (2) -> Listar os dados de todos os clientes\n");
    printf(" (3) -> Listar os dados de um cliente especifico\n");
    printf(" (4) -> Sair\n");
    printf(" (5) -> Deleta os arquivos\n");
    
    int option, repeat = 1;

    int root, promo_rrn = NIL;
    KEYPAGE promo_key;
    int promoted;

    //root = RRN da pagina que representa a root

    while(repeat != 0)
    {
        int repeatKey = false;
        printf("\n\nOpcao: ");
        scanf("%d", &option);
        switch (option)
        {
        case 1:
            
            KEYPAGE insertDataKey;
            strcpy(insertDataKey.Id, insertData[inseridos].Id.ClientId);
            strcat(insertDataKey.Id, insertData[inseridos].Id.MovieId);
            insertDataKey.rrn = 0;

            //tento acessar a arvore, se ela existe eu leio o header (root) e tento inserir a chave no index, se ela nao existir eu crio ela colocando o register na root (createRoot)
            if(access("bTreeIndex.bin", F_OK ) == 0) {
                file = fopen("bTreeIndex.bin", "r+b");
                fread(&root, sizeof(int), 1, file);

                //tento inserir a chave no indice 
                //repeatKey = 
                insertDataKey.rrn = getRegisterRrn();
                promoted = insertRegisterIndex(root, insertDataKey, &promo_rrn, &promo_key, file); 
                if(promoted == 3) {
                    printf("Chave %s duplicada!\n", insertDataKey);
                    fclose(file);
                    inseridos ++;
                    break;
                }

                if(promoted)
                   root = createRoot(promo_key, root, promo_rrn, file);

            } else {
                file = fopen("bTreeIndex.bin", "w+b");
                int x = NIL;
                fwrite(&x, 1, sizeof(int), file);
                root = createRoot(insertDataKey, -1, -1, file);
                repeatKey = false;
            }
            fclose(file);

            insertRegister(insertData[inseridos]);
           
            inseridos++;
            savePosition();

            break;

        case 2:
            
	        file = fopen("bTreeIndex.bin", "r+b");
            fread(&root, sizeof(int), 1, file);
   
            PrintTreeInOrder(root, file);

            fclose(file);

            break;

        case 3:
            int found;
            KEYPAGE findDataKey;
            strcpy(findDataKey.Id, buscaData[buscas].ClientId);
            strcat(findDataKey.Id, buscaData[buscas].MovieId);

            file = fopen("bTreeIndex.bin", "r+b");
            fread(&root, sizeof(int), 1, file);

            found = findRegister(root, findDataKey, file);
            if(!found)
                printf("Nao foi possivel localizar a chave \"%s\"!\n",  findDataKey.Id);

            fclose(file);
            
            buscas++;
            savePosition();
            
            break;

        case 4:
            savePosition();

            printf("Finalizando...\n\n\n");
            repeat = 0;
            break;

        case 5: 
            remove("bTreeIndex.bin");
            remove("position.bin");
            remove("registers.bin");
            printf("Arquivos resetados!\n");
            fopen("registers.bin", "w+b");

            inseridos = 0;
            buscas = 0;
            break;
        default:
            break;
        }
    }

    return 0;
}