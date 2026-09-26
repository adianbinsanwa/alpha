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
    TT_END,
    TT_NUM,
    TT_HEX,
    TT_BIN,
    
    TT_WORD,
    TT_CHAR,
    TT_BOOL,
    TT_NONE,
    
    TT_COMMA,
    TT_ENDLN,
    TT_SCOPE,
    TT_KW_BE,
    TT_KW_AS,
    TT_KW_OR,
    TT_KW_IF,
    
    TT_STRING,
    TT_LPARAM,
    TT_RPARAM,
    TT_LBRACK,
    TT_RBRACK,
    TT_PADDIN,
    TT_KW_LET,
    TT_KW_AND,
    TT_KW_NOT,
    TT_KW_RET,
    TT_KW_FOR,
    TT_KW_SET,
    
    TT_KW_FUNC,
    TT_KW_ELSE,
    TT_KW_FROM,
    
    TT_KEYWORD,
    
    TT_KW_WHILE,
    
    TT_IDENTIFIER
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

Tnode *tok_expand_paddin(Tnodes *t, size_t left, size_t right);

bool tok_check(String *s, Tnode *tok, char *target, bool match_all);

#endif //TOKENS_H

//files tokens.h is included
/* 
 * main.h
 * tools.h
 * lexer.h
 * parser.h
 * decoder.h
 */

