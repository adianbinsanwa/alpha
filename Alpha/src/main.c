#include "../header/main.h"

int main() {
    Package p=p_open("../../newfile.a");
    if (lexer(&p) ) {
        parser(&p);
        //toks_print(&p.source, &p.tnode);
        d_print_ast(&p.tnode, 0);
    }
    d_print_errs(&p);
    p_close(&p);
    return 0;
}