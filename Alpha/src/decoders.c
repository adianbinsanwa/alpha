#include "../header/decoder.h"

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


char *
d_tok_errtype(TE_Msg mtype) {
    switch (mtype) {
        case TEM_UNCLOSED_BRACK:
             return "      unclosed brackets";
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
_print_ast_rec(Tnodes *t, size_t idx, const char *prefix, bool is_last) {
    if ((idx == 0 && t->size <= 1) || (idx >= t->size - 1) ) return;                /* past last real node */

    Tnode *n = t_get(t, idx);

    /* print the current node */
    printf("%s%s", prefix, is_last ? "└── " : "├── ");
    printf("Node no: %zu", idx);

    /* optional: also show the token type (very useful while debugging) */
    printf("  (%s)\n", d_toktype(n->type));
    //print_tok(s, n->start, n->width);
    
    /* prepare the prefix for the children */
    char child_prefix[256];
    snprintf(child_prefix, sizeof(child_prefix), "%s%s", prefix, is_last ? "    " : "│   ");

    /* treat left and right as the two children
       (this matches how you currently wire the tree) */
    bool has_left  = n->left  != 0;
    bool has_right = n->right != 0;

    if (has_left) {
        _print_ast_rec(t, n->left,  child_prefix, !has_right);
    }
    if (has_right) {
        _print_ast_rec(t, n->right, child_prefix, true);
    }
}


void
d_print_ast(Tnodes *t, size_t root) {
    if (!t || !t->val || t->size < 2) {
        printf("(empty AST)\n");
        return;
    }

    /* root line (no ├── / └──) */
    Tnode *n = t_get(t, root);
    printf("Node no: %zu  (%s)\n", root, d_toktype(n->type) );

    char prefix[256] = "";
    bool has_left  = n->left  != 0;
    bool has_right = n->right != 0;

    if (has_left)
        _print_ast_rec(t, n->left,  prefix, !has_right);
    if (has_right)
        _print_ast_rec(t, n->right, prefix, true);
}


void
d_print_errs(Package *p) {
    Err *err;
    for (size_t i=0; i <= p->e_size; ++i) {
        err=&p->errors[i];
        printf("%s:%zu:%zu: ", p->filename, err->tok->row, err->tok->column);
        printf("error:\n%s\n      ", d_tok_errtype(err->mtype) );
    }
};