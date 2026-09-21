#ifndef TOKENS_H
#define TOKENS_H

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

#include "dynamic_string.h"

extern bool match_all;
extern bool match_within;


typedef enum {
    TE_INTERNAL,
    TE_ERROR,
} TE_Type;


typedef enum {
    TEM_UNCLOSED_BRACK,
} TE_Msg;


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
    size_t capacity;
} Tnodes;


Tnodes t_open();

bool t_push(Tnodes *tns, Tnode tn);

bool t_pull(Tnodes *t);

Tnode *t_get(Tnodes *t, size_t pos);

void t_close(Tnodes *t);

//##########-token_funcs-############//

Tnode tok_node(size_t start, size_t width, size_t type, size_t row, size_t column);

Tnode tok_expand_paddin(void);

bool tok_check(String *s, size_t start, size_t width, char *target, bool match_all);

#endif //TOKENS_H

//files tokens.h is included
/* 
 * main.h
 * tools.h
 * lexer.h
 * parser.h
 * decoder.h
 */

