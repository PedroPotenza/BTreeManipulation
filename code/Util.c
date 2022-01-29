#include "../header.h"

extern int inseridos;
extern int buscas;

/*-------------------------------------- Utils --------------------------------------*/
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
