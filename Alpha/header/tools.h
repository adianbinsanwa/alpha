#ifndef TOOLS_H
#define TOOLS_H

#include <stddef.h>
#include "tokens.h"
#include "dynamic_string.h"

#define MAX_ERR 20


typedef struct {
    Tnode *tok;
    size_t tpos;
    size_t twidth;
    TE_Type type;
    TE_Msg mtype;
} Err;


typedef struct {
    char *filename;
    String source;
    Tnodes tnode;
    Err errors[MAX_ERR];
    size_t e_size;
} Package;


Package p_open(char *filename);

void p_close(Package *p);

Tnode *get_last_tok(Tnodes *t, size_t spos);


#endif //TOOLS_H

//files tools.h is included

/*
 * lexer.h
 * parser.h
 * decoder.h
 */