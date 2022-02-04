#ifndef HEADER_H
#define HEADER_H

#define true 1
#define false 0
#define NIL -1
#define MAXKEYS 3
#define NOKEY '@'
#define PAGESIZE 47
#define REGISTERSIZE 156

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int inseridos = 0;
int buscas = 0;

typedef struct s_Key {
    char ClientId[3];
    char MovieId[3];
} KEY;

typedef struct s_KeyPage {
    char Id[5];
    int rrn;
} KEYPAGE;

typedef struct s_Register {
    KEY Id;
    char ClientName[50];
    char MovieName[50];
    char Genre[50];
} REGISTER;

typedef struct {
    int keyCount;
    KEYPAGE keys[3];
    int childs[4];
} PAGE;


#include "code/Insert.c"
#include "code/Util.c"

FILE * readPositions();
void savePosition();

void insertRegister(REGISTER newRegister);

int insertRegisterIndex(int rrn, KEYPAGE key, int* promo_right_child, KEYPAGE* promo_key, FILE* file);
int createRoot(KEYPAGE key, int left, int right, FILE* file);
int getPage(FILE* file);
void initPag(PAGE* page);
void writeIndex(int rrn, PAGE* page, FILE* file);
void readPage (int rrn, PAGE* page, FILE* file);
int searchNode(KEYPAGE key, PAGE* page, int *pos);
void insertInPage (KEYPAGE key, int rightChild, PAGE* page);
void split (KEYPAGE key, int rightChild, PAGE* oldPage, KEYPAGE* promo_key, int* promo_right_child, PAGE* newPage, FILE* file);



#endif