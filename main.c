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
    
    int option, repeat = 1, inserted = 0, found = 0;

    while(repeat != 0)
    {
        printf("\n\nOpcao: ");
        scanf("%d", &option);

        switch (option)
        {
        case 1:
            
            insertRegister(insertData[inseridos]);
            inseridos++;

            savePosition();

            break;

        case 2:
            //found = PrimarySearchV2(buscaPrimariaData[buscas_primarias]);
            
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