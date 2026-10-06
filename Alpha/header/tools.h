#ifndef TOOLS_H
#define TOOLS_H
#define get_last_rtok(x, y) get_last_tok((x), (y), false)
#define get_last_ltok(x, y) get_last_tok((x), (y), true)

#include <stddef.h>
#include "tokens.h"
#include "dynamic_string.h"



typedef struct {
    Tnode *tok;
    size_t tpos;
    size_t twidth;
    TE_Type type;
    TE_Msg mtype;
} Err;


typedef struct {
    const char *const filename;
    String source;
    Tnodes tnode;
    Err *errors;
    size_t e_size;
} Package;


Package p_open(const char *const filename);

void p_close(Package *p);

Tnode *get_last_tok(Tnodes *t, const size_t spos, bool is_left);

void e_push(Package *p, Tnode *tok, size_t tpos, size_t twidth, TE_Type type, TE_Msg message);

void loop_set(const size_t val);

bool loop_step(void);

#endif //TOOLS_H

//files tools.h is included

/*
 * lexer.h
 * parser.h
 * decoder.h
 */