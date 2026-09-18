#include "../header/tokens.h"


void
print_tok(String *s, size_t pos, size_t width) {
    for (size_t i=0; i < width; ++i) {
        if (s->ptr[pos+i]=='\n') {
            printf("\\newline");
        } else {
            printf("%c", s->ptr[pos+i]);
        }
    }
};


Tnodes
t_open() {
    return (Tnodes){.val=NULL, .size=1};
};


void
t_close(Tnodes *t) {
    free(t->val);
    t->val=NULL;
    t->size=1;
};


Tnode
t_node(size_t start, size_t width, size_t type, size_t row, size_t column) {
    return (Tnode){.start=start, .width=width, .type=type, .row=row, .column=column, .left=0, .right=0};
};


Tnode *
t_get(Tnodes *t, size_t pos) {
    if (pos > t->size-2) {
        return &t->val[t->size-1];
    }
    return &t->val[pos];
};


Tnode
t_expand_paddin(void) {
    return (Tnode){.type=TT_PADDIN};
};



void
tok_print(String *s, Tnode *t) {
    printf("Tnode:-(\n    val: ");
    
    print_tok(s, t->start, t->width);
    
    printf("\n    type: %s\n    pos: %zu:%zu\n    width: %zu\n    left: %zu\n    right: %zu\n)\n", d_toktype(t->type), t->row, t->column, t->width, t->left, t->right);
};


void
toks_print(String *s, Tnodes *t) {
    for (size_t i=0; i < t->size; ++i) {
        printf("no: %zu\n ", i);
        tok_print(s, &t->val[i]);
    }
};


void
t_error(String *s, size_t pos, size_t width, size_t tpos, size_t twidth) {
    /*size_t start= ? : ;
    size_t end= ? : ;
    print_tok(s, start, end)*/
};


bool
t_push(Tnodes *tns, Tnode tn) {
    Tnode *new=realloc(tns->val, (tns->size + 1) * sizeof(*tns->val) );
    if (!new) {
        return false;
    }
    tns->val=new;
    tns->val[tns->size-1]=tn;
    tns->val[tns->size++]=(Tnode){.type=TT_END};
    return true;
};


bool
tok_check(String *s, size_t strt, size_t width, char *target, bool match_all) {
    if (!match_all) {
        for (size_t i=0; i < width-1; ++i) {
            if (!s_member_of(target, s->ptr[strt + i]) ) {
                return false;
            }
        }
        return true;
    } else if (width != strlen(target) ) {
        return false;
    }
    for (size_t i=0; i < width-1; ++i) {
        if (s->ptr[strt + i]!= target[i]) {
            return false;
        }
    }
    return true;
};

char *
d_toktype(TnodeType type) {
    switch (type) {
        case TT_END:     
             return "END";
        case TT_NUM:     
             return "NUM";
        case TT_WORD:    
             return "WORD";
        case TT_CHAR:    
             return "CHAR";
        case TT_BOOL:    
             return "BOOL";
        case TT_NONE:    
             return "NONE";
        case TT_COMMA:   
             return "COMMA";
        case TT_ENDLN:  
             return "ENDLN";
        case TT_STRING:  
             return "STRING";
        case TT_LPARAM:  
             return "LPARAM";
        case TT_RPARAM:  
             return "RPARAM";
        case TT_LBRACK:  
             return "LBRACK";
        case TT_RBRACK:  
             return "RBRACK";
        case TT_PADDIN:  
             return "PADDIN";
        case TT_KEYWORD:
             return "KEYWORD";
        case TT_SCOPE:
             return "SCOPE";
        case TT_IDENTIFIER:
             return "IDENTIFIER";
        default:         
             return "invalid tokentype";
    }
};



