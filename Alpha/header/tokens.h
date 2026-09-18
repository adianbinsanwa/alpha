#ifndef TOKENS_H
#define TOKENS_H

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

#include "dynamic_string.h"

typedef enum {
    TT_END,//0
    TT_NUM,//1
    
    TT_WORD,//2
    TT_CHAR,//3
    TT_BOOL,//4
    TT_NONE,//5
    
    TT_COMMA,//6
    TT_ENDLN,//7
    TT_SCOPE,//8
    
    TT_STRING,//9
    TT_LPARAM,//10
    TT_RPARAM,//11
    TT_LBRACK,//12
    TT_RBRACK,//13
    TT_PADDIN,//14
    TT_KEYWORD,//15
    TT_IDENTIFIER//16
} TnodeType;


typedef struct {
    size_t start;
    size_t width;
    size_t type;
    size_t row;
    size_t column;
    size_t left;
    size_t right;
} Tnode;


typedef struct {
    Tnode *val;
    size_t size;
} Tnodes;


Tnodes t_open();

char *d_toktype(TnodeType type);

void tok_print(String *s, Tnode *t);

void t_close(Tnodes *t);

Tnode t_node(size_t start, size_t width, size_t type, size_t row, size_t column);

Tnode *t_get(Tnodes *t, size_t pos);

Tnode t_expand_paddin(void);

void t_print(String *s, size_t pos, size_t width);

//void t_error(String *s, size_t pos, size_t width);

bool t_push(Tnodes *tns, Tnode tn);

bool tok_check(String *s, size_t strt, size_t width, char *target, bool match_all);

void toks_print(String *s, Tnodes *t);
    
#endif //TOKENS_H