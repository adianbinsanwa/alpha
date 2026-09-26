#ifndef DECODER_H
#define DECODER_H

#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "dynamic_string.h"
#include "tokens.h"
#include "tools.h"


char *d_boolean(int res);

char *d_toktype(TnodeType type);

void d_print_ast(String *s, Tnodes *t, size_t root);

void d_print_toks(String *s, Tnodes *t, size_t pos, size_t lim);

void d_print_errs(Package *p);

#endif //DECODER_H

//files decoder.h is included 

/*
 * main.h
 */