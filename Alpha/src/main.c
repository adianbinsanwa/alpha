#include "../header/main.h"

int main() {
    
    Package p=p_open("../../newfile.txt");
    
    if (lexer(&p) && parser(&p) && !p.e_size ) {
        d_print_toks(&p.source, &p.tnode, 0, 0);
        d_print_ast(&p.source, &p.tnode, 0);
        printf("%s", p.source.ptr);
    }
    d_print_errs(&p);
    p_close(&p);
    return 0;
}