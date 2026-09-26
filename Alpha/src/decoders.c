#include "../header/decoder.h"

//##########-implementation_funcs-###########//


void
_tok_print(String *s, size_t pos, size_t width) {
    for (size_t i=0; i < width; ++i) {
        //0//
        if (*s_get(s, pos + i)=='\n') {
            //1//
            printf("\\newline");
            continue;
        }
        printf("%c", *s_get(s, pos + i) );
    }
    if (width==0) {
        //0//
        printf("NULL");
    }
};



char *
_tok_errtype(TE_Msg mtype) {
    switch (mtype) {
        case TEM_UNCLOSED_BRACK:
             return "expected '}'";
        default:
             return "      <empty>";
    }
};


/* Pretty-print the AST as a tree (like the `tree` command).
 * Starts at the given root index and walks left / right.
 * Example output:
 *
 * Node no: 0
 * ├── Node no: 1
 * │   └── Node no: 4
 * └── Node no: 2
 */
static void
_print_ast_rec(String *s, Tnodes *t, size_t idx, const char *prefix, bool is_last) {
    if ((idx == 0 && t->size <= 1) || (idx >= t->size) ) return;                /* past last real node */

    Tnode *n = t_get(t, idx);

    /* print the current node */
    printf("%s%s", prefix, is_last ? "└── " : "├── ");
    printf("Node: %zu", idx);
    
    /* optional: also show the token type (very useful while debugging) */
    if (n->type==TT_IDENTIFIER || n->type==TT_WORD || n->type==TT_HEX || n->type==TT_BIN || n->type==TT_NUM) {
        //0//
        printf(" (");
        _tok_print(s, n->start, n->width);
        printf(")\n");
    } else {
        //0//
        printf(" (%s)\n", d_toktype(n->type) );
    }
    /* prepare the prefix for the children */
    char child_prefix[256];
    snprintf(child_prefix, sizeof(child_prefix), "%s%s", prefix, is_last ? "    " : "│   ");

    /* treat left and right as the two children
       (this matches how you currently wire the tree) */
    bool has_right = n->right != 0;
    bool has_left = n->left != 0;
    

    if (has_left) {
        //0//
        printf("%s%s\n", child_prefix, "│");
        _print_ast_rec(s, t, n->left,  child_prefix, !has_right);
    }
    if (has_right) {
        //0//
        printf("%s%s\n", child_prefix, "│");
        _print_ast_rec(s, t, n->right, child_prefix, true);
    }
}


//##########-surface_funcs-############//


char *
d_boolean(int res) {
    switch (res) {
        case 0:
             return "False";
        case 1:
             return "True";
        default:
             return "invalid bool";
    }
};


char *
d_toktype(TnodeType type) {
    switch (type) {
        case TT_BIN:
             return "BINARY";
        case TT_HEX:
             return "HEX";   
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
        case TT_SCOPE:
             return "SCOPE";
        case TT_KW_BE:
             return "KW_BE";
        case TT_KW_AS:
             return "KW_AS";
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
        case TT_KW_LET:
             return "KW_LET";
        case TT_KW_SET:
             return "KW_SET";
        case TT_KW_AND:
             return "KW_AND";
        case TT_KW_NOT:
             return "KW_NOT";
        case TT_KW_FOR:
             return "KW_FOR";
        case TT_KW_RET:
             return "KW_RET";
        case TT_KEYWORD:
             return "KEYWORD";
        case TT_IDENTIFIER:
             return "IDENTIFIER";
        default:         
             return "invalid tokentype";
    }
};


void
d_print_ast(String *s, Tnodes *t, size_t root) {
    /* root line (no ├── / └──) */
    Tnode *n = t_get(t, root);
    printf("Node: %zu  (%s)\n", root, d_toktype(n->type) );

    char prefix[256] = "";
    bool has_right = n->right != 0;

    if (n->left  != 0) {
        //0//
        printf("%s\n", "│");
        _print_ast_rec(s, t, n->left,  prefix, !has_right);
    }
    if (has_right) {
        //0//
        printf("%s\n", "│");
        _print_ast_rec(s, t, n->right, prefix, true);
    }
}


void
d_print_toks(String *s, Tnodes *t, size_t pos, size_t lim) {
    Tnode *tok=t_get(t, pos);
    
    printf("no: %zu\n    val: ", pos);
    _tok_print(s, tok->start, tok->width);
    
    printf("\n    type: %s\n    row: %zu\n    column: %zu\n    left: %zu\n    right: %zu\n\n", d_toktype(tok->type), tok->row, tok->column, tok->left, tok->right);
    if (lim - 1!=0 && tok->type != TT_END) {
        //0//
        d_print_toks(s, t, pos + 1, lim - 1);
    }
};


void
_print_errs(Package *p, size_t pos) {
    Err *err=&p->errors[pos];
    
    printf("%s:%zu:%zu: ", p->filename, err->tok->row, err->tok->column);
    
    printf("%s:\n      %s\n    %zu | ", err->type==TE_ERROR ? "error" : "internal_error" , _tok_errtype(err->mtype), err->tok->row );
    
    size_t line_start= err->tok->start - err->tok->column;
    
    _tok_print(&p->source, line_start, err->tok->column + err->tok->width);
    printf("\n        ");
    
    for (size_t i=0; i < err->tpos + err->tok->column; ++i) {
        printf(" "); //0//
    }
    for (size_t i=0; i < err->twidth; ++i) {
        printf("^"); //0//
    }
    
    
    if (pos != p->e_size - 1) {
        //0//
        _print_errs(p, pos + 1);
    }
};

void
d_print_errs(Package *p) {
    if (p->e_size==0) {
        return;
    }
    _print_errs(p, 0);
};