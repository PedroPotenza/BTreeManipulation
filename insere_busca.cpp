#include<stdio.h>
#include<conio.h>
#include<string.h>

int main() {
    FILE *fd;
    
    //////////////////////////////   
    struct CliF {
        char CodCli[3];
        char CodF[3];
        char NomeCli[50];
        char NomeF[50];
        char Genero[50];
    } vet[13] = {{"01", "01", "Joao da Silva", "Filme-1", "Aventura"},              
                {"01", "02", "Joao da Silva", "Filme-2", "Aventura"},
				{"01", "03", "Joao da Silva", "Filme-3", "Aventura"},
				
				{"02", "03", "Pedro Silva e Silva", "Filme-3", "Aventura"},
				{"02", "03", "Pedro Silva e Silva", "Filme-3", "Aventura"},//duplicada

                {"01", "01", "Pedro Silva e Silva", "Filme-5", "Comedia"},//duplicada

				{"02", "01", "Pedro Poteza", "Filme-4", "Comedia"},
                {"02", "02", "Lets", "Filme-6", "Comedia"},

                {"01", "04", "Nome0104", "Filme-do-0104", "Comedia"},
                {"01", "05", "Nome0105", "Filme-do-0105", "Comedia"},

                {"01", "06", "Nome0106", "Filme-do-0106", "Comedia"},
                {"01", "07", "Nome0107", "Filme-do-0107", "Comedia"},

                {"01", "08", "Nome0108", "Filme-do-0108", "Comedia"}
                
                };
       
    fd = fopen("insere.bin", "w+b");
    fwrite(vet, sizeof(vet), 1, fd);
    fclose(fd);
    
    //////////////////////////////
	struct busca {
        char CodCli[3];
        char CodF[3];
    } vet_b[3] = {{"01","01"},
                  {"01","03"},
				  {"05", "07"}};//n�o encontrada
    
    fd = fopen("busca.bin", "w+b");
    fwrite(vet_b, sizeof(vet_b), 1, fd);
    fclose(fd);    
}

