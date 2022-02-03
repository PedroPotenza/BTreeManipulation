#include "../header.h"

/**
 * @brief Insere um registro no final do arquivo "registers.bin"
 * 
 * @param newRegister registro a ser inserido 
 */
void insertRegister(REGISTER newRegister) {

    FILE* dataFile = fopen("registers.bin", "r+b");
    fseek(dataFile, 0, SEEK_END);

    printf("Id Cliente: %s\n", newRegister.Id.ClientId);
    printf("Id Filme: %s\n", newRegister.Id.MovieId);
    printf("Nome Cliente: %s\n", newRegister.ClientName);
    printf("Nome Filme: %s\n", newRegister.MovieName);
    printf("Genero: %s\n", newRegister.Genre);

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
    close(dataFile);
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


int createRoot(char* key, int left, int right, FILE* file) { 
    PAGE page; 
    int rrn;

    rrn = getPage(file);
    initPage(&page);
    // printf("Criou a pagina %d e inicializou ela com tudo nulo.\n", rrn);

    strcpy(page.keys[0].Id, key);
    page.keys[0].rrn = 0;
    page.childs[0] = left;
    page.childs[1] = right;
    page.keyCount = 1;
    printf("Adicionou o registro na pagina %d.\n", rrn);

    writeIndex(rrn, &page, file);
    printf("Gravou a pagina no  bTreeIndex.bin\n");

    rewind(file);
    fwrite(&rrn, 1, sizeof(int), file);
    printf("Reescrevendo o header para %d.\n", rrn);

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

// /**
//  * @brief Divide a pagina e sobe o da esquerda
//  * 
//  * @param key chave a ser inserida 
//  * @param rightChild filho da direita dela 
//  * @param oldPage pagina antiga (q tem que ser dividida)
//  * @param promo_key chave a ser promovida
//  * @param promo_righ_child o filho da chave a ser promovida
//  * @param newPage a nova pagina feita com metade da pagina antiga
//  */
// void split (KEYPAGE key, int rightChild, PAGE* oldPage, KEY* promo_key, int* promo_right_child, PAGE* newPage, FILE* file) {
    
//     int i; 
//     //cria 2 vetores auxiliares, um com as chaves e o outro com os filhos dessas chaves
//     KEY workKeys[3 + 1];
//     int workChild[3 + 2];

//     //salvo as informacoes da pagina antiga nessa variavel local
//     for (i = 0; i < 3; i++)
//     {
//         strcpy(workKeys[i].ClientId, oldPage->keys[i].ClientId);
//         strcpy(workKeys[i].MovieId, oldPage->keys[i].MovieId);

//         workChild[i] = oldPage->childs[i];
//     }
    
//     //nao esqueco de salvar o ultimo filho (pq tem uma posicao a mais)
//     workChild[i] = oldPage->childs[i];

//     char keyLook[5];
//     char keyPage[5];

//     strcpy(keyLook, key.ClientId);
//     strcat(keyLook, key.MovieId);

//     strcpy(keyPage, workKeys[2].ClientId);
//     strcat(keyPage, workKeys[2].MovieId);

//     int compareResult = strcmp(keyLook,keyPage);

//     //insiro a chave na pagina auxiliar ja de forma ordenada 
//     for (i = 3; compareResult < 0 && i > 0; i--) {

//         strcpy(keyPage, workKeys[i-1].ClientId);
//         strcat(keyPage, workKeys[i-1].MovieId);

//         compareResult = strcmp(keyLook,keyPage);
        
//         strcpy(workKeys[i].ClientId, workKeys[i-1].ClientId);
//         strcpy(workKeys[i].MovieId, workKeys[i-1].MovieId);
//         workChild[i+1] = workChild[i];
//     }
//     strcpy(workKeys[i].ClientId, key.ClientId);
//     strcpy(workKeys[i].MovieId, key.MovieId);
//     workChild[i+1] = rightChild;

//     //salva o rrn da pagina nova a ser criada
//     *promo_right_child = getPage(file);
//     //cria a pagina
//     initPage(newPage);

//     //preencher a pagina velha e a pagina nova com as informacoes corretas

//     strcpy(oldPage->keys[0].ClientId, workKeys[0].ClientId);
//     strcpy(oldPage->keys[0].MovieId, workKeys[0].MovieId);
//     oldPage->childs[0] = workChild[0];
//     for (int i = 1; i <= 2; i++)
//     {
//         strcpy(oldPage->keys[i].ClientId, "@@");
//         strcpy(oldPage->keys[i].MovieId, "@@");
//         oldPage->childs[i] = NIL;
//     }

//     char keyPromo[5];

//     strcpy(keyPromo, workKeys[1].ClientId);
//     strcat(keyPromo, workKeys[1].MovieId);

//     printf("Chave %s promovida!\n", keyPromo);

//     for (int i = 2; i <= 3; i++)
//     {
//         strcpy(newPage->keys[i-2].ClientId, workKeys[i].ClientId);
//         strcpy(newPage->keys[i-2].MovieId, workKeys[i].MovieId);
//         newPage->childs[i-2] = workChild[i];
//     }

//     newPage->keyCount = 2;
//     *promo_key = workKeys[1];

//     printf("==================== VELHA ===========================\n");
//     printf("informacoes da pagina antiga:\n");
//     printf("key Count: %d\n", oldPage->keyCount);
//         printf("\tChilds[0]: %d\n", oldPage->childs[0]);
//     printf("key [0] id.Client: %s\n", oldPage->keys[0].ClientId);
//     printf("key [0] id.Movie: %s\n", oldPage->keys[0].MovieId);
//         printf("\tChilds[1]: %d\n", oldPage->childs[1]);
//     printf("key [1] id.Client: %s\n", oldPage->keys[1].ClientId);
//     printf("key [1] id.Movie: %s\n", oldPage->keys[1].MovieId);
//         printf("\tChilds[2]: %d\n", oldPage->childs[2]);
//     printf("key [2] id.Client: %s\n", oldPage->keys[2].ClientId);
//     printf("key [2] id.Movie: %s\n", oldPage->keys[2].MovieId);
//         printf("\tChilds[3]: %d\n", oldPage->childs[3]);
//     printf("\n");

//     printf("==================== NOVA ============================\n");

//     printf("informacoes da pagina %d:\n");
//     printf("key Count: %d\n", newPage->keyCount);
//         printf("\tChilds[0]: %d\n", newPage->childs[0]);
//     printf("key [0] id.Client: %s\n", newPage->keys[0].ClientId);
//     printf("key [0] id.Movie: %s\n", newPage->keys[0].MovieId);
//         printf("\tChilds[1]: %d\n", newPage->childs[1]);
//     printf("key [1] id.Client: %s\n", newPage->keys[1].ClientId);
//     printf("key [1] id.Movie: %s\n", newPage->keys[1].MovieId);
//         printf("\tChilds[2]: %d\n", newPage->childs[2]);
//     printf("key [2] id.Client: %s\n", newPage->keys[2].ClientId);
//     printf("key [2] id.Movie: %s\n", newPage->keys[2].MovieId);
//         printf("\tChilds[3]: %d\n", newPage->childs[3]);
//     printf("\n");
    
// }

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

    printf("insertRegisterIndex com rrn \"%d\"\n", rrn);

    // se for uma folha
    if(rrn == -1) {

        printf("entrou no if do rrn -1\n");

        *promo_key = key;
        *promo_right_child = NIL;
        return(1);

    }

    //le a pagina daquele rrn, colocando ela na memória (carregando a variavel page)
    readPage(rrn, &page, file);
    
    //tenta achar a chave dentro da pagina
    found = searchNode(key, &page, &pos); 
    if(found) { 
        return 0;
    }

    promoted = insertRegisterIndex(page.childs[pos], key, &rrnPromotedBelow, &keyPromotedBelow, file);
    
    printf("promoted: %d\n", promoted);
    //se a chave foi inserida na pagina, ela nao foi promovida, logo para a recursao
    if(!promoted) {
        return false;
    }

    //se tem espaco naquela pagina, insere la 
    if(page.keyCount < MAXKEYS) {

        printf("tem espaco na pagina \"%d\"\n", rrn);
        // printf("keyPromotedBelow.ClientId: %s\n", keyPromotedBelow.ClientId);
        // printf("keyPromotedBelow.MovieId: %s\n", keyPromotedBelow.MovieId);
        printf("rrnPromotedBelow: %d", rrnPromotedBelow);

        insertInPage(keyPromotedBelow, rrnPromotedBelow, &page);
        writeIndex(rrn, &page, file);
        return false;
    } else { 
        //se nao tem espaco na pagina, divide ela 
        printf("Divisao de no.\n");
        // split(keyPromotedBelow, rrnPromotedBelow, &page, promo_key, promo_right_child, &newpage, file);
        printf("AAAAAAA RRN: %d\n", rrn);
        // writeIndex(rrn, &page, file);
        // writeIndex(*promo_right_child, &newpage, file);
        return true;
    }

}