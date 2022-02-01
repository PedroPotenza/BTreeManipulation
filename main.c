/*
    Alunos:
        Gabriel Tetzlaf Mansano - 201150956
        Pedro Henrique Potenza Fernandes - 201151219
*/

#include "header.h"


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
    
    int option, repeat = 1;

    int root, promo_rrn = NIL;
    KEY promo_key;

    //root = RRN da pagina que representa a root

    while(repeat != 0)
    {
        int repeatKey = false;
        printf("\n\nOpcao: ");
        scanf("%d", &option);

        switch (option)
        {
        case 1:
            
            //tento acessar a arvore, se ela existe eu leio o header (root) e tento inserir a chave no index, se ela nao existir eu crio ela colocando o register na root (createRoot)
            if(access("bTreeIndex.bin", F_OK ) == 0) {
                file = fopen("bTreeIndex.bin", "r+b");
                fread(&root, sizeof(int), 1, file);

                //tento inserir a chave no indice 
                repeatKey = insertRegisterIndex(root, insertData[inseridos].Id, &promo_rrn, &promo_key, file); 
                //retorno booleano: true se a chave ja existir, falso se a chave for uma nova chave nao existente

            } else {
                file = fopen("bTreeIndex.bin", "w+b");
                int x = NIL;
                fwrite(&x, 1, sizeof(int), file);
                printf("teste\n");
                root = createRoot(insertData[inseridos].Id, -1, -1, file);
                repeatKey = false;
            }
            fclose(file);

            // so inserir o registro no dataFile se a chave nao existir no indice
            if(!repeatKey)
                insertRegister(insertData[inseridos]);
            else {
                
                char key[5];
                strcpy(key, insertData[inseridos].Id.ClientId);
                strcat(key, insertData[inseridos].Id.MovieId);
                printf("Chave %s duplicada!\n", key);
            }

            inseridos++;
            savePosition();

            break;

        case 2:
            //found = PrimarySearchV2(buscaPrimariaData[buscas_primarias]);
            printf("teste\n");
            KEY chavesTest[5];
            char keyComplete[2][5];
            int compareResult;

            strcpy(chavesTest[0].ClientId, "01");
            strcpy(chavesTest[0].MovieId, "02");

            strcpy(keyComplete[0], chavesTest[0].ClientId);
            strcat(keyComplete[0], chavesTest[0].MovieId);

            strcpy(chavesTest[1].ClientId, "04");
            strcpy(chavesTest[1].MovieId, "01");

            strcpy(keyComplete[1], chavesTest[1].ClientId);
            strcat(keyComplete[1], chavesTest[1].MovieId);

            compareResult = strcmp(keyComplete[0],keyComplete[1]);

            if(compareResult < 0)
                printf("A chave %s eh menor que a chave %s", keyComplete, keyComplete[1]);
            if(compareResult > 0)
                printf("A chave %s eh maior que a chave %s", keyComplete[0], keyComplete[1]);
            if(compareResult == 0)
                printf("As chaves %s e %s sao iguais", keyComplete[0], keyComplete[1]);

            break;

        case 3:
            //found = SecondarySearch(buscaSecundariaData[buscas_secundarias]);
            
            buscas++;
            savePosition();
            
            break;

        case 4:
            savePosition();

            printf("Finalizando...\n\n\n");
            repeat = 0;
            break;
        
        default:
            break;
        }
    }

    return 0;
}