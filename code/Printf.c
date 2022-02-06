#include "../header.h"

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

/*

    readPage(rrn, &pagina)

    if(page.child[0] == -1)
        printa a pagina inteira ate o countKey
        return;
    else
        PrintTreeInOrder(page.child[0]);

    for(int i = 0 ate countkey da pagina) {
        printa o registro da pagina [i]
        PrintTreeInOrder(page.child[i+1]);
    }

*/