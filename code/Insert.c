#include "../header.h"

int insertRegister(REGISTER newRegister) {

    
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

}