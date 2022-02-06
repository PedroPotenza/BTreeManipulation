#include "../header.h"

void PrintTreeInOrder(int rrn, FILE* file) { 

    PAGE page;

    readPage(rrn, &page, file);

    if(page.childs[0] == -1) {
        printf("printa a pagina inteira");
        return;
    }
    else 
        PrintTreeInOrder(page.childs[0], file);

    for (int i = 0; i < page.keyCount; i++)
    {
        printf("printa o registro [i]");
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