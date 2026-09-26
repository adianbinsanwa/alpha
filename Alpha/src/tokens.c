#include "../header/tokens.h"


bool match_all=true;
bool match_within=false;

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
    return (Tnodes){.val=NULL, .size=1, .capacity=0};
};


void
t_close(Tnodes *t) {
    free(t->val);
    t->val=NULL;
    t->size=1;
    t->capacity=0;
};


Tnode
tok_node(size_t start, size_t width, size_t type, size_t row, size_t column) {
    return (Tnode){.start=start, .width=width, .type=type, .row=row, .column=column, .left=0, .right=0};
};


Tnode *
t_get(Tnodes *t, size_t pos) {
    if (pos > t->size-2) {
        return &t->val[t->size-1];
    }
    return &t->val[pos];
};


Tnode *
tok_expand_paddin(Tnodes *t, size_t left, size_t right) {
    Tnode paddin= tok_node(0, 0, TT_PADDIN, 0, 0);
    
    paddin.left=left; 
    paddin.right=right;
    
    if (!t_push(t, paddin) ) {
        return t_get(t, t->size - 1);
    }
    return t_get(t, t->size - 2);
};


Tnode *
tok_new_scope(Tnodes *t) {
    if (!t_push(t, tok_node(0, 0, TT_SCOPE, 0, 0) ) ) {
        return t_get(t, t->size - 1);
    }
    return t_get(t, t->size - 2);
};

bool
t_push(Tnodes *tns, Tnode tn) {
    if (!tns->capacity) {    
        Tnode *new=realloc(tns->val, (tns->size + 1) * sizeof(*tns->val) );
        if (!new) {
            return false;
        }
        tns->val=new;
    } else {
        --tns->capacity;
    }
    tns->val[tns->size-1]=tn;
    tns->val[tns->size++]=tok_node(0, 0, TT_END, 0, 0);
    return true;
};


bool
t_pull(Tnodes *t) {
    if (!t->val) {
        return false;
    }
    t->val[--t->size-1]=tok_node(0, 0, TT_END, 0, 0);
    ++t->capacity;
    return true;
};

bool
tok_check(String *s, Tnode *tok, char *target, bool match_all) {
    char word[tok->width];
    
    for (size_t i=0; i < tok->width; ++i) {
        word[i]=*s_get(s, tok->start + i);
    }
    word[tok->width]='\0';
    return match_all ? s_eq(word, target) : s_has_only_from(word, target);
};

