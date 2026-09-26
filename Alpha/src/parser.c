#include "../header/parser.h"


bool parse_tok(Package *p, size_t *pos, Tnode *tok);

bool parse_param(Package *p, const size_t prev, size_t *pos);

void
e_push(Package *p, Tnode *tok, size_t tpos, size_t twidth, TE_Type type, TE_Msg message) {
   if (p->e_size!=MAX_ERR - 1) {
      p->errors[p->e_size++]=(Err){.tok=tok, .tpos=tpos, .twidth=twidth, .type=type, .mtype=message};
   } 
};


bool
_is_valid_tok(TnodeType type) {
   return type!=TT_SCOPE && type!=TT_PADDIN;
} 


bool
parse_scope(Package *p, size_t *pos, size_t scope_base, TnodeType end) {
   Tnode *tok;
   size_t current=0;
   while ((tok=t_get(&p->tnode, *pos) )->type!=end && tok->type!=TT_END) {
      //0//
      //printf("%zu, %s\n", t_get(&p->tnode, 1)->right, d_toktype(tok->type) );
      
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
              //printf("%zu\n", tok->right);
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
   size_t last_pos=*pos, next= (*pos) + 1;
   
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
parse_param_var(Package *p, const size_t prev, size_t *pos) {
   size_t ltok, ctok;
   Tnode *nxt;
   
   while ((nxt= t_get(&p->tnode, *pos) )->type!=TT_END && _is_valid_tok(nxt->type) ) {
      //0//
      size_t current=*pos;
      if (nxt->type==TT_RPARAM) {
         //1//
         if (!t_get(&p->tnode, ctok)->right) {
            //2//
            printf("issue\n");
            return false;
         }
         get_last_tok(&p->tnode, prev)->left=ctok;
         get_last_tok(&p->tnode, prev)->right=*pos;
         
         break;
      } else if (nxt->type==TT_WORD && !ctok) {
         //1//
         
      } else if (tok_check(&p->source, t_get(&p->tnode, *pos), "as", match_all) && !ctok) {
         //1//
         if (!ltok) {
            //2//
            printf("erroor: %zu, %zu\n", ltok, *pos);
            return false;
         }  
         nxt->left=ltok;
         nxt->type=TT_KW_AS;
         ctok=*pos;
      } else {
         //1//
         printf("isssue\n");
         return false;
      }
   }
   if (nxt->type!=TT_RPARAM) {
      return false;
   }
   return true;
};


bool
parse_var(Package *p, Tnode *tok, size_t prev, size_t *pos) {
   tok->type= *s_get(&p->source, tok->start)=='s' ? TT_KW_SET : TT_KW_LET;
   
   int limit= 3, ppos= 0;
   size_t pcs[limit];
   bool parse_next= false;
   
   while ((tok=t_get(&p->tnode, *pos) )->type!=TT_END ) {
      //0//
      size_t current= *pos;
      
      if (tok->type==TT_ENDLN || tok->type==TT_COMMA || ppos == limit || !_is_valid_tok(tok->type) ) {
         //1//
         break;
      } else if (parse_next) {
         //1//
         if (!parse_tok(p, pos, tok) ) {
            //2//
            break;
         } else if (ppos==2) {
            t_get(&p->tnode, pcs[1])->right=current;
         }
      } else if (tok->type==TT_LPARAM) {
         //1//
         ++(*pos);
         if (!parse_param(p, current, pos) ) {
            return false;
         }
      } else if (tok_check(&p->source, tok, "be", match_all) ) {
         //1//
         tok->type= TT_KW_BE;
         tok->left=pcs[0];
         t_get(&p->tnode, prev)->left= *pos;
         parse_next=true;
      } else {
         printf("issues\n");
         return false;
      }
      pcs[ppos]= current;
      ++ppos;
      ++(*pos);
   }
   if (ppos==1) {
      //0//
      t_get(&p->tnode, prev)->left=pcs[0];
   } else if (!ppos) {
      //0//
      printf("eroror\n");
      return false;
   }
   //t_get(&p->tnode, prev)->left= p->tnode.size - 3;
   return true;
};


bool
parse_param(Package *p, const size_t prev, size_t *pos) {
   size_t ctok=0, ltok=0;
   Tnode *nxt;
   
   while ((nxt= t_get(&p->tnode, *pos) )->type!=TT_END && _is_valid_tok(nxt->type) ) {
      //0//
      size_t current=*pos;
      
      if (nxt->type==TT_RPARAM) {
         //1//
         if (!t_get(&p->tnode, ctok)->right) {
            //2//
            printf("issue\n");
            return false;
         }
         get_last_tok(&p->tnode, prev)->left=ctok;
         get_last_tok(&p->tnode, prev)->right=*pos;
         
         break;
      } else if (tok_check(&p->source, t_get(&p->tnode, *pos), "as", match_all) && !ctok) {
         //1//
         if (!ltok) {
            //2//
            printf("erroor: %zu, %zu\n", ltok, *pos);
            return false;
         }  
         nxt->left=ltok;
         nxt->type=TT_KW_AS;
         ctok=*pos;
      } else if (nxt->type==TT_WORD) {
         //1//
         if (!ctok) {
            ltok=*pos;
         } else {
            get_last_tok(&p->tnode, ctok)->right=*pos;
         }
         nxt->type=TT_IDENTIFIER;
      } else if (nxt->type==TT_COMMA || nxt->type==TT_ENDLN) {
         //1//
         if (!t_get(&p->tnode, ctok)->right) {
            printf("issue\n");
            return false;
         }
         nxt->type=TT_PADDIN;
         
         get_last_tok(&p->tnode, prev)->left=ctok;
         get_last_tok(&p->tnode, prev)->right=*pos;
         
         ctok=0;
         ltok=0;
      } else {
         printf("errrrroor:- %s\n", d_toktype(nxt->type) );
         return false;
      }
      
      ++(*pos);
   }
   if (nxt->type!=TT_RPARAM) {
      printf("unclosed param\n");
      return false;
   }
   return true;
};


bool
parse_brack(Package *p, Tnode *tok, size_t *pos) {
   if (!t_push(&p->tnode, tok_node(0, 0, TT_SCOPE, 0, 0) ) ) {
      //0//token push
      printf("exception: pushing 'tnode'\n");
      return false;
   }
   while (t_get(&p->tnode, *pos)->type!=TT_RBRACK ) {
      ++(*pos);
   }
   return true;
};


//##############-main_hub-#############//


bool
parse_tok(Package *p, size_t *pos, Tnode *tok) {
   size_t prev=(*pos)++;
   //++(*pos);
   
   if (tok_check(&p->source, tok, "import", match_all) ) {
      //0//
      return parse_import(&p->tnode, tok, pos);
   } else if (tok->type==TT_LBRACK) {
      //0//
      return parse_brack(p, tok, pos);
   } else if (tok->type==TT_LPARAM) {
      //0//
      return parse_param(p, prev, pos);
   } else if (tok_check(&p->source, tok, "let", match_all) || tok_check(&p->source, tok, "set", match_all) ) {
      //0//
      return parse_var(p, tok, prev, pos);
   }
   return true;
};

