#include "../header/parser.h"


static bool parse_tok(Package *p, size_t *pos, Tnode *tok);



static inline bool
_is_valid_tok(TnodeType type) {
   return type!=TT_SCOPE && type!=TT_PADDIN && type!=TT_END;
} 


static inline bool
_is_linebreak(TnodeType type) {
   return type==TT_ENDLN || type==TT_COMMA;
};


static inline bool
_is_literal(TnodeType type) {
   return type==TT_NUM || type==TT_BIN || type==TT_HEX || type==TT_STRING || type==TT_CHAR;
};


static void
_set_childs(Tnodes *t, const size_t left, const size_t pos, const size_t right) {
   Tnode *tok= t_get(t, pos);
   tok->right= right;
   tok->left= left;
};



static bool
parse_scope(Package *p, size_t *pos, size_t scope_base, bool is_brack) {
   /*
    * parses the current scope
    */
   TnodeType end= is_brack ? TT_RBRACK : TT_END; 
   
   Tnode *tok;
   while ((tok=t_get(&p->tnode, *pos) )->type!=end && tok->type!=TT_END) {
      //0//
      size_t current= *pos;
      //d_print_toks(&p->source, &p->tnode, current, 1);
      
      switch (tok->type) {
         //1//
         case TT_SCOPE:
         case TT_PADDIN:
              break;
         case TT_ENDLN:
         case TT_COMMA:
              if (get_last_rtok(&p->tnode, scope_base)->left ) {
                 //2//
                 tok->type=TT_PADDIN;
                 get_last_rtok(&p->tnode, scope_base)->right=*pos;
              }
              break;
         default: 
              if (!parse_tok(p, pos, tok) ) return false;
         
              get_last_rtok(&p->tnode, scope_base)->left=current;
              continue;
      }
      ++(*pos);
   }   
   if (tok->type!=end) {
      printf("unclosed '{' got: %s, expected: %s\n", d_toktype(tok->type), d_toktype(end) );
      d_print_toks(&p->source, &p->tnode, *pos, 1);
      return false;
   }
   get_last_rtok(&p->tnode, scope_base)->right=*pos;
   return true;
};

//##########-main_entry-############//

bool
parser(Package *p) {
   /*
    * main entry point for parser
    */
   size_t pos=1;
   return parse_scope(p, &pos, 0, false);
};

//##########-parse_parts-###########//


static bool
parse_import(Package *p, Tnode *tok, const size_t prev, size_t *pos) {
   /*
    * parses import statements
    */
   tok->type= TT_KW_IMPORT;
   
   while (_is_valid_tok((tok= t_get(&p->tnode, *pos) )->type) ) {
      //0//
      size_t current= *pos;
      
      if (tok->type==TT_ENDLN) {
         //1//
         if (!get_last_rtok(&p->tnode, prev)->left) {
            printf("!!!$$\n");
            return false;
         }
         break;
      } else if (tok->type==TT_COMMA) {
         //1//
         get_last_rtok(&p->tnode, prev)->right= current;
      } else if (tok->type==TT_WORD) {
         //1//
         get_last_rtok(&p->tnode, prev)->left= current;
         tok->type= TT_IDENTIFIER;
      } else {
         //1//
         printf("!!!$hhhh$\n");
         return false;
      }
      ++(*pos);
   }
   
   return true;
};


static bool
parse_param_val(Package *p, const size_t prev, size_t *pos) {
   /*
    * parses param with value
    * e.g:- (a, 'b', 10, func() )
    */
   Tnode *nxt;
   
   while (_is_valid_tok((nxt= t_get(&p->tnode, *pos) )->type) ) {
      //0//
      size_t current= *pos;
      
      if (nxt->type==TT_RPARAM) {
         //1//
         if (!get_last_rtok(&p->tnode, prev)->left) {
            //2//
            printf("error:\nfrom: parse_param_var\nissue: no ')'\n");
            return false;
         }
         get_last_rtok(&p->tnode, prev)->right= current;
         ++(*pos);
         break;
      } else if (_is_literal(nxt->type) || nxt->type==TT_WORD) {
         //1//
         if (!parse_tok(p, pos, nxt) ) {
            //2//
            printf("!!!helll\n");
            return false;
         }
         get_last_rtok(&p->tnode, prev)->left= current;
         continue;
      } else if (_is_linebreak(nxt->type) ) {
         //1//
         get_last_rtok(&p->tnode, prev)->right= *pos;
         nxt->type= TT_PADDIN;
      } else {
         //1//
         printf("daamn issues: %s\n", d_toktype(nxt->type) );
         return false;
      }
      ++(*pos);
   }
   if (nxt->type!=TT_RPARAM) {
      printf("!!????: %s\n", d_toktype(nxt->type) );
      return false;
   }
   return true;
};



static bool
parse_param_var(Package *p, const size_t prev, size_t *pos) {
   /*
    * parses variable declaration param
    * e.g let '(x, y, z as const int)' be (10, 20, 30)
    */
   
   int8_t ppos= 0;
   const int8_t limit= 3, sp_vars= 0, sp_As= 1, sp_type= 2;
   size_t pcs[limit];
   
   loop_set(2);
   
   Tnode *nxt;
   while (_is_valid_tok((nxt= t_get(&p->tnode, *pos) )->type) ) {
      //0//
      size_t current=*pos;
      
      d_print_toks(&p->source, &p->tnode, current, 1);
      
      if (!loop_step() ) {
         break;
      }
      if (nxt->type==TT_RPARAM) {
         //1//
         if (ppos < limit) {
            printf("daammmmm\n");
            d_print_toks(&p->source, &p->tnode, 0, 0);
            return false;
         }
         ++ppos;
         break;
      } else if (ppos == sp_As && tok_check(&p->source, nxt, "as", t_match_all) && get_last_rtok(&p->tnode, pcs[sp_vars])->type!=TT_PADDIN ) {
         //1//
         nxt->type= TT_KW_AS;
         pcs[ppos++]= current;
         
      } else if (nxt->type==TT_WORD) {
         //1//
         nxt->type= TT_IDENTIFIER;
         if (ppos <= sp_As) {
            //2//
            pcs[sp_vars]= ppos== sp_vars ? current : pcs[sp_vars];
            get_last_rtok(&p->tnode, pcs[sp_vars])->right= current;
            
         }
         ++ppos;
      } else if (_is_linebreak(nxt->type) && ppos == sp_As) {
         //1//
         nxt->type= TT_PADDIN;
         get_last_rtok(&p->tnode, pcs[sp_vars])->right= current;
      } else {
         //1//
         printf("isssue\n");
         d_print_toks(&p->source, &p->tnode, current, 1);
         return false;
      }
      ++(*pos);
   }
   if (ppos==limit) {
      Tnode *lparam= t_get(&p->tnode, prev);
      lparam->left= pcs[sp_As];
      lparam->right= *pos;
   }
   return true;
};


static bool
parse_var(Package *p, Tnode *tok, size_t prev, size_t *pos) {
   /*
    * parses variable declaration and definition statements
    * e.g 
    * set (x, u) be (2, 's')
    * let (x as int) be 10
    */
   tok->type= *s_get(&p->source, tok->start)=='s' ? TT_KW_SET : TT_KW_LET;
   
   int8_t ppos= 0;
   
   const int8_t limit= 3, sp_vars = 0, sp_Be = 1, sp_vals = 2;// sp= structure position
   size_t pcs[limit];
   //bool parse_next= false;
   
   while (_is_valid_tok((tok=t_get(&p->tnode, *pos) )->type) && !_is_linebreak(tok->type) && ppos != limit) {
      //0//
      size_t current= *pos;
      
      //printf("%s, %zu\n", d_toktype(tok->type), *pos);
      if (ppos == sp_vals) {
         //1//
         if (!parse_tok(p, pos, tok) ) {
            //2//
            return false;
         } else {
            //2//
            t_get(&p->tnode, pcs[sp_Be])->right= current;
         }
         continue;
      } else if (tok->type==TT_LPARAM && ppos== sp_vars) {
         //1//
         ++(*pos);
         if (!parse_param_var(p, current, pos) ) {
            //2//
            printf("isssuesss,,\n");
            return false;
         }
      } else if (tok_check(&p->source, tok, "be", t_match_all && ppos == sp_Be) ) {
         //1//
         tok->type= TT_KW_BE;
         tok->left=pcs[sp_vars];
         
         t_get(&p->tnode, prev)->left= *pos;
         
      } else {
         //1//
         printf("issuesf\n");
         d_print_toks(&p->source, &p->tnode, *pos, 1);
         return false;
      }
      pcs[ppos++]= current;
      ++(*pos);
   }
   /*if (ppos==) {
      //0//
      t_get(&p->tnode, prev)->left=pcs[0];
   } else*/ 
   if (!ppos || ppos != limit) {
      //0//
      printf("eroror\n");
      return false;
   }
   printf("%s, %zu\n", d_toktype(tok->type), *pos);
   return true;
};


static bool
parse_param_arg(Package *p, const size_t prev, size_t *pos) {
   /*
    * parses param arguments
    * e.g (y as int, z as bool, u as float)
    */
   size_t ctok=0, ltok=0;
   Tnode *nxt;
   
   while (_is_valid_tok((nxt= t_get(&p->tnode, *pos) )->type) ) {
      //0//
      size_t current=*pos;
      
      if (nxt->type==TT_RPARAM) {
         //1//
         if (ctok && !t_get(&p->tnode, ctok)->right) {
            //2//
            printf("issue1\n");
            return false;
         }
         get_last_rtok(&p->tnode, prev)->left= ctok;
         get_last_rtok(&p->tnode, prev)->right= current;
         
         break;
      } else if (tok_check(&p->source, t_get(&p->tnode, *pos), "as", t_match_all) && !ctok) {
         //1//
         if (!ltok) {
            //2//
            printf("erroor: %zu, %zu\n", ltok, *pos);
            return false;
         }  
         nxt->left= ltok;
         nxt->type= TT_KW_AS;
         
         ctok= current;
      } else if (nxt->type==TT_WORD) {
         //1//
         if (!ctok) {
            ltok= current;//2//
         } else {
            //2//
            get_last_rtok(&p->tnode, ctok)->right=*pos;
         }
         nxt->type= TT_IDENTIFIER;
      } else if (_is_linebreak(nxt->type) && ctok) {
         //1//
         if (!t_get(&p->tnode, ctok)->right) {
            //2//
            printf("issue2\n");
            return false;
         }
         nxt->type= TT_PADDIN;
         
         get_last_rtok(&p->tnode, prev)->left= ctok;
         get_last_rtok(&p->tnode, prev)->right= current;
         
         ctok=0;
         ltok=0;
      } else if (nxt->type!=TT_ENDLN) {
         //1//
         printf("errrrroor:- %s\n", d_toktype(nxt->type) );
         return false;
      }
      ++(*pos);
   }
   if (nxt->type!=TT_RPARAM) {
      //0//
      printf("unclosed param\n");
      return false;
   }
   return true;
};



static bool
parse_brack(Package *p, Tnode *tok, const size_t prev, size_t *pos, bool post_inc) {
   /*
    * parses local scope
    * e.g {
    * 
    * func(7)
    * }
    */
   if (!tok_new_scope(&p->tnode) ) {
      //0//token push
      printf("exception: pushing 'tnode'\n");
      return false;
   }
   tok->right= p->tnode.size - 2;
   
   bool ret= parse_scope(p, pos, tok->right, true);
   
   if (post_inc) ++(*pos);
   return ret;
};



static bool
parse_func(Package *p, Tnode *tok, const size_t prev, size_t *pos) {
   /*
    * parses function
    */
   tok->type= TT_KW_FUNC;
   
   int8_t ppos= 0;//current pcs pos
   
   //structure points
   const int8_t limit= 4, sp_name = 0, sp_param = 1, sp_Return = 2, sp_body = 3;//parts limit // sp= structure position// func name, func param, func return, func body
   size_t pcs[limit];//[0] = <func_name>, [1] = arguments, [2] = "return" keyword, [3] = return type, [4] = function body
   
   
   while (_is_valid_tok((tok= t_get(&p->tnode, *pos) )->type) && ppos != limit ) {
      //0//
      size_t current= *pos;
      
      if (ppos == sp_body) {
         //1//
         if (tok->type== TT_LBRACK) {
            //2//if '{' and ppos is at 4
            ++(*pos);//pre inc pos
            if (!parse_brack(p, tok, current, pos, false) ) {
               //3//
               return false;
            }
            
            t_get(&p->tnode, pcs[sp_name])->right= current;
         } else if (tok->type == TT_WORD) {
            //2//
            get_last_rtok(&p->tnode, pcs[sp_Return])->right= current;
            
            tok->type= TT_IDENTIFIER;
         } 
         if (_is_linebreak(tok->type) || tok->type == TT_IDENTIFIER) {
            //2//
            ++(*pos);
            continue;
         }
      } else if (tok_check(&p->source, tok, "return", t_match_all) && ppos == sp_Return) {
         //1//
         t_get(&p->tnode, pcs[sp_name])->left= current;
         
         tok->left= pcs[sp_param];
         tok->type= TT_KW_RET;
      } else if (tok->type == TT_WORD) {
         //1//
         t_get(&p->tnode, prev)->left= current;
         
         tok->type= TT_IDENTIFIER;
      } else if (tok->type== TT_LPARAM && ppos == sp_param) {
         //1//if '(' and ppos is at 1
         ++(*pos);//pre inc pos
         if (!parse_param_arg(p, current, pos) ) {
            printf("issues..\n");
            return false;
         }
      } else {
         //1//if unknown token
         printf("damn isssues!\n");
         return false;
      }
      pcs[ppos++]= current;//store the current segment in pcs + inc ppos
      ++(*pos);//inc pos
   }
   if (ppos != limit) {
      printf("damn issues !!?\n");
      return false;
   }
   return true;
};



static bool
parse_flags(Package *p, Tnode *tok, const size_t prev, size_t *pos) {
   if (!parse_tok(p, pos, t_get(&p->tnode, *pos) ) ) {
      printf("!!error!!\n");
      return false;
   }
   t_get(&p->tnode, prev + 1)->right= prev;
   return true;
};



static bool
parse_struct(Package *p, Tnode *tok, const size_t prev, size_t *pos) {
   tok->type= TT_KW_STRUCT;
   
   int8_t ppos= 0;
   
   const int8_t limit= 2, sp_name= 0, sp_body= 1;
   size_t pcs[limit];
   
   while (_is_valid_tok((tok= t_get(&p->tnode, *pos) )->type) && !_is_linebreak(tok->type) && ppos != limit) {
      //0//
      size_t current= *pos;
      
      if (tok->type==TT_WORD && ppos == sp_name) {
         //1//
         tok->type= TT_IDENTIFIER;
         get_last_ltok(&p->tnode, prev)->left= current;
         
         pcs[ppos++]= current;
      } else if (tok->type==TT_LPARAM && ppos <= sp_body) {
         //1//
         ++(*pos);
         if (!parse_param_arg(p, current, pos) ) {
            //2//
            printf("!!!issssuusu\n");
            return false;
         }
         get_last_ltok(&p->tnode, prev)->left= current;
         
         pcs[ppos++]= current;
      } else {
         printf("invalid stuff\n");
         return false;
      }
      ++(*pos);
   }
   
   return true;
};


//##############-main_hub-#############//


static bool
parse_tok(Package *p, size_t *pos, Tnode *tok) {
   /*
    * central hub for routing statements
    */
   const size_t prev=(*pos)++;
   
   if (tok_check(&p->source, tok, "import", t_match_all) ) {
      //0//
      return parse_import(p, tok, prev, pos);
   } else if (tok->type==TT_LBRACK) {
      //0//
      return parse_brack(p, tok, prev, pos, true);
   } else if (tok->type==TT_LPARAM) {
      //0//
      return parse_param_val(p, prev, pos);
   } else if (tok_check(&p->source, tok, "let", t_match_all) || tok_check(&p->source, tok, "set", t_match_all) ) {
      //0//
      return parse_var(p, tok, prev, pos);
   } else if (tok_check(&p->source, tok, "func", t_match_all) ) {
      //0//
      return parse_func(p, tok, prev, pos);
   } else if (tok_check(&p->source, tok, "public", t_match_all) ) {
      //0//
      return parse_flags(p, tok, prev, pos);
   } else if (tok_check(&p->source, tok, "struct", t_match_all) ) {
      //0//
      return parse_struct(p, tok, prev, pos);
   } else if (tok->type==TT_WORD) {
      //0//
      tok->type= TT_IDENTIFIER;
   }
   return true;
};

