#ifndef ERROR_MANAGER_H
#define ERROR_MANAGER_H

#include<stdio.h>
#include<stdlib.h>
#include<stddef.h>

typedef enum {
    EP_LEXER,
    EP_PARSER,
} ERR_PLACE;


typedef enum {
    ET_NONE,
    ET_FILE,
    ET_INTERNAL,
    ET_CODE,
} ERR_TYPE;


typedef struct {
    ERR_PLACE location;
    ERR_TYPE type;
} Exec_Res;



#endif //ERROR_MANAGER_H