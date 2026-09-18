#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include "dynamic_string.h"
#include "error_manager.h"
#include "tokens.h"
#include "decoder.h"



typedef struct {
    String *source;
    Tnodes *tnode;
} Package;


bool lexer(char *filename, Package *pack);

#endif //LEXER_H