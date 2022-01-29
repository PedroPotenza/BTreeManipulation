#ifndef HEADER_H
#define HEADER_H

#define true 1;
#define false 0;
#define NIL -1;

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

typedef struct s_Register {
    KEY Id;
    char ClientName[50];
    char MovieName[50];
    char Genre[50];
} REGISTER;

#include "code/Insert.c"
#include "code/Util.c"

FILE * readPositions();
void savePosition();

int insertRegister(REGISTER newRegister);


#endif