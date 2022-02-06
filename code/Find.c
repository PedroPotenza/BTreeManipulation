#include "../header.h"

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