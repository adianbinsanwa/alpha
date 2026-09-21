#include "../header/tools.h"


Package
p_open(char *filename) {
    return (Package){.filename=filename, .source=s_open(), .tnode=t_open(), .e_size=0};
};

void
p_close(Package *p) {
    s_close(&p->source);
    t_close(&p->tnode);
};







Tnode * 
get_last_tok(Tnodes *t, size_t spos) {
   Tnode *tok=t_get(t, spos);
   while (tok->right) {
      tok=t_get(t, tok->right);
   }
   return tok;
};





