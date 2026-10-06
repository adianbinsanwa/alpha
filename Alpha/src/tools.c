#include "../header/tools.h"

#define MAX_LIMIT 5
#define MAX_ERR 20

size_t counter= 0;
size_t limit;
Err errors[MAX_ERR];



void
loop_set(const size_t val) {
    limit= val;
}


bool
loop_step(void) {
    ++counter;
    return counter != limit && counter != MAX_LIMIT;
}




Package
p_open(const char *const filename) {
    return (Package){.filename=filename, .source=s_open(), .tnode=t_open(), .errors= errors, .e_size=0};
};



void
p_close(Package *p) {
    s_close(&p->source);
    t_close(&p->tnode);
};





Tnode * 
get_last_tok(Tnodes *t, const size_t spos, bool is_left) {
   Tnode *tok=t_get(t, spos);
   
    while ((!is_left && tok->right) || (is_left && tok->left) ) {
      tok= t_get(t, is_left ? tok->left: tok->right);
   }
   return tok;
};


void
e_push(Package *p, Tnode *tok, size_t tpos, size_t twidth, TE_Type type, TE_Msg message) {
   if (p->e_size!=MAX_ERR - 1) {
      p->errors[p->e_size++]=(Err){.tok=tok, .tpos=tpos, .twidth=twidth, .type=type, .mtype=message};
   } 
};