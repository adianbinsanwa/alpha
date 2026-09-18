#include "../header/parser.h"



Tnode * 
get_last_tok(Tnodes *t) {
   Tnode *tok=t_get(t, 0);
   while (tok->right) {
      tok=t_get(t, tok->right);
   }
   return tok;
};


size_t
tok_resolve(Tnodes *t) {
   for (size_t i=1;; ++i) {
      Tnode *tok=t_get(t, i);
      if (tok->type==TT_SCOPE || tok->type==TT_END || tok->type==TT_PADDIN) {
         return i - 1;
      }
   }
};


size_t
get_pack_end(Tnodes *t, size_t last_pos) {
   TnodeType target=t_get(t, last_pos)->type==TT_LPARAM ? TT_RPARAM : TT_RBRACK;
   size_t pos=last_pos;
   ++pos;
   
   Tnode *tok;
   while ((tok=t_get(t, pos) )->type!=TT_END) {
      if (tok->type==target) {
         return pos;
      } else if (tok->type==TT_LBRACK || tok->type==TT_LPARAM) {
         pos=get_pack_end(t, pos);
         if (pos==last_pos) {
            break;
         }
      } 
      ++pos;
   };
   
   return last_pos;
};


void
parse_import(Tnodes *t, Tnode *tok, size_t *pos) {
   size_t next= (*pos) + 1;
   
   tok->left=next;
   tok->type=TT_KEYWORD;
   
   Tnode *next_node=t_get(t, next);
   if (next_node->type==TT_WORD) {
      //2//
      next_node->type=TT_IDENTIFIER;
   }
   get_last_tok(t)->left=*pos;
   ++(*pos);
};


bool
parser(char *filename, Package *pack) {
    Package p=*pack;
    
    size_t pos=0;
    
    bool ierr=false;
    
    Tnode *tok;
    while ((tok=t_get(p.tnode, pos) ) ) {
        //0//
        if (tok->type==TT_END) {
            //1//
            get_last_tok(p.tnode)->right=pos;
            break;
        } else if (tok_check(p.source, tok->start, tok->width, "import", true) ) {
            //1//
            parse_import(p.tnode, tok, &pos);
        } else if (tok->type==TT_ENDLN || tok->type==TT_COMMA) {
           //1//
           if (!get_last_tok(p.tnode)->left || tok_resolve(p.tnode)==pos) {
              ++pos;
              continue;
           } else if (!t_push(p.tnode, t_node(0, 0, TT_PADDIN, 0, 0) ) ) {
              //3//token push
              printf("exception: pushing 'tnode'\n");
              return ierr;
           } 
           get_last_tok(p.tnode)->right=p.tnode->size-2;
        } else if (tok->type==TT_LBRACK || tok->type==TT_LPARAM) {
           size_t end=get_pack_end(p.tnode, pos);
           if (pos==end) {
              break;
           }
           pos=end;
           printf("start:-\n ");
           tok_print(p.source, tok);
           printf("end:-\n ");
           tok_print(p.source, t_get(p.tnode, pos) );
           
        }
        ++pos;
    }
    if (tok->type!=TT_END) {
       printf("%s:%zu:%zu: error: unclosed bracket\n   ", filename, tok->row, tok->column);
       tok_print(p.source, tok);
    } else {
       toks_print(p.source, p.tnode);
    }
    return true;
};



int main() {
    String s=s_open();
    Tnodes t=t_open();
    char *file="../newfile.a"; 
   
    Package p={.source=&s, .tnode=&t};
    
    if (lexer(file, &p) ) {
        parser(file, &p);
    }
    s_close(&s);
    t_close(&t);
}

