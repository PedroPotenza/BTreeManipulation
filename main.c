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
            //found = SecondarySearch(buscaSecundariaData[buscas_secundarias]);
            
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