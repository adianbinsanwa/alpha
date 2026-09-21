#include "../header/parser.h"


bool parse_tok(Package *p, size_t *pos, Tnode *tok);


void
e_push(Package *p, Tnode *tok, size_t tpos, size_t twidth, TE_Type type, TE_Msg message) {
   if (p->e_size!=MAX_ERR - 1) {
      p->errors[p->e_size++]=(Err){.tok=tok, .tpos=tpos, .twidth=twidth, .type=type, .mtype=message};
   } 
};


bool
parse_scope(Package *p, size_t *pos, size_t scope_base, TnodeType end) {
   Tnode *tok;
   size_t current=0;
   while ((tok=t_get(&p->tnode, *pos) )->type!=end && tok->type!=TT_END) {
      //0//
      current=*pos;
      switch (tok->type) {
         case TT_SCOPE:
         case TT_PADDIN:
              break;
         case TT_ENDLN:
         case TT_COMMA:
              if (get_last_tok(&p->tnode, scope_base)->left ) {
                 tok->type=TT_PADDIN;
                 get_last_tok(&p->tnode, scope_base)->right=*pos;
              }
              break;
         default: 
              if (!parse_tok(p, pos, tok) ) {
                  return false;
              }
              get_last_tok(&p->tnode, scope_base)->left=current;
              continue;
      }
      ++(*pos);
   }   
   if (tok->type!=end && end!=TT_END) {
      return false;
   }
   get_last_tok(&p->tnode, scope_base)->right=*pos;
   return true;
};

//##########-main_entry-############//

bool
parser(Package *p) {
   size_t pos=1;
   return parse_scope(p, &pos, 0, TT_END);
};

//##########-parse_parts-###########//


bool
parse_import(Tnodes *t, Tnode *tok, size_t *pos) {
   size_t last_pos=*pos;
   size_t next= (*pos) + 1;
   
   tok->left=next;
   tok->type=TT_KEYWORD;
   
   Tnode *next_node=t_get(t, next);
   if (next_node->type==TT_WORD) {
      //2//
      //next_node->type=TT_IDENTIFIER;
   }
   ++(*pos);
   return true;
};


bool
parse_var(Package *p, Tnode *tok, size_t *pos) {
   
   while ((tok=t_get(&p->tnode, *pos) )->type!=TT_END ) {
      if (tok->type==TT_ENDLN || tok->type==TT_COMMA) {
         break;
      }
      ++(*pos);
   }
   return true;
};


bool
parse_brack(Package *p, Tnode *tok, size_t *pos, bool isbrack) {
   if (!t_push(&p->tnode, tok_node(0, 0, TT_SCOPE, 0, 0) ) ) {
      //0//token push
      printf("exception: pushing 'tnode'\n");
      return false;
   }
   tok->right=p->tnode.size-2;
   if (!parse_scope(p, pos, tok->right, isbrack ? TT_RBRACK : TT_RPARAM) ) {
      e_push(p, tok, 0, 0, TE_ERROR, TEM_UNCLOSED_BRACK);
   }
   ++(*pos);
   return true;
};


bool
parse_tok(Package *p, size_t *pos, Tnode *tok) {
   ++(*pos);
   if (tok_check(&p->source, tok->start, tok->width, "import", match_all) ) {
      //1//
      return parse_import(&p->tnode, tok, pos);
   } else if (tok->type==TT_LBRACK || tok->type==TT_LPARAM) {
      return parse_brack(p, tok, pos, tok->type==TT_LBRACK);
   } else if (tok_check(&p->source, tok->start, tok->width, "let", match_all) || tok_check(&p->source, tok->start, tok->width, "set", match_all) ) {
      return parse_var(p, tok, pos);
   }
   return true;
};

