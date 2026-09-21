#include "../header/lexer.h"

//#include <errno.h> #include <string.h>

/* 
 * incomplete stuff:
 *     column at :- lexer(),
 *     proper word checking at :- word_type(),
 */

bool char_scanner(FILE *pf, Package *p);

TnodeType
symbol_type(char symb) {
    switch (symb) {
        case '(': 
             return TT_LPARAM;
        case ')': 
             return TT_RPARAM;
        case ',': 
             return TT_COMMA;
        case '\n': 
             return TT_ENDLN;
        case '{': 
             return TT_LBRACK;
        case '}': 
             return TT_RBRACK;
        case '\'':
             return TT_CHAR;
        case '"':
             return TT_STRING;
        default:
             return 0;
    }
};



TnodeType
word_type(String *s, size_t pos, size_t width) {
    return tok_check(s, pos, width, s_0_9, match_within) ? TT_NUM : TT_WORD;
};

size_t
get_column(size_t a, size_t b) {
    return a;
};


bool
check_last_toktype(Tnodes *t, TnodeType istype) {
    return t->val[t->size-2].type==istype;
};

//###############-entry_point-#############//

bool
lexer(Package *p) {
    //open file
    FILE *pfile= fopen(p->filename, "r");
    
    if (!pfile) {
        //0//if file not open
        perror(p->filename);
        return false;
    }
    return char_scanner(pfile, p);//tokenize
};

//############-tokenizer-#############//

bool
char_scanner(FILE *pf, Package *p) {
    /*load file and manage line ends
     *split words/chars and pack tokens
     *pos tracks the normalized source
     *column/row track the original input
    */
    //system
    
    size_t pos=0;//current pos throughout the whole file string
    size_t column=0;//column count
    size_t row=1;//row count
    //helpers
    char quote='\0';//quote holder indicating in quotes + which quote "=2/'=1/none=0
    size_t w_size=0;//counts the size of the word
    size_t w_start;//holds the start pos of a word
    
    //error to return when something internal goes wrong
    bool ierr=false;
    
                
    if (!t_push(&p->tnode, tok_node(0, 0, TT_SCOPE, 0, 0) ) ) {
        //0//token push
        printf("exception: pushing 'tnode'\n");
        return ierr;
    }
    
    int a;
    while ((a=fgetc(pf) ) != EOF) {
        //0//
        char atom=(char)a;//cast int a to char
        
        if (s_push(&p->source, atom)!=SS_NO_ERR) {
            //1//source push
            printf("exception: pushing 'source'\n");
            return ierr;
        }
        if (quote!='\0') {
            //1//if in quotes
            ++w_size;//inc word size
            if (atom==quote) {
                //2//if quote terminated
                Tnode new=tok_node(w_start, w_size, atom=='\'' ? TT_CHAR : TT_STRING, row, get_column(column, w_size) );
                
                if (!t_push(&p->tnode, new) ) {
                    //3//token push
                    printf("exception: pushing 'tnode'\n");
                    return ierr;
                }
                w_size=0;//reset word size
                quote='\0';//terminate quote
            } else if (atom=='\\') {
                //2//if \(char)
                int next=fgetc(pf);//if '\' then pull 1 character after '\' in the string/char 
                if (next==EOF) {
                    break;//3//break if EOF
                }
                if (s_push(&p->source, (char)next)!=SS_NO_ERR) {
                    //3//source push
                    printf("exception: pushing 'source'\n");
                    return ierr;
                }
                //aggresive update
                ++w_size;
                ++pos;
            }
        } else if (s_member_of(" \n(){},", atom) ) {
            //1//if valid symbols
            if (w_size) {
                //2//if previous not appended
                Tnode new=tok_node(w_start, w_size, word_type(&p->source, w_start, w_size), row, get_column(column, w_size) );
                
                if (!t_push(&p->tnode, new) ) {
                    //3//token push
                    printf("exception: pushing 'tnode'\n");
                    return ierr;
                }
                w_size=0;//reset size
            }
            if (atom!=' ') {
                //2//allow everything except ' '
                if (atom=='\n' && (check_last_toktype(&p->tnode, TT_ENDLN) || check_last_toktype(&p->tnode, TT_COMMA) ) ) {
                    s_pull(&p->source);//3//if an newline already appended
                    --pos;//restore the unnecessary incremented pos
                } else {
                    //3//
                    Tnode new=tok_node(pos, 1, symbol_type(atom), row, column);
                
                    if (!t_push(&p->tnode, new) ) {
                        //4//token push
                        printf("exception: pushing 'tnode'\n");
                        return ierr;
                    }
                }
            } else {
                s_pull(&p->source);//2//if space then remove it
                //update column
                continue;//continue without incrementing pos
            }
            if (atom=='\n') {
                column=0;//2//reset column
                ++row;//update row
            }
        } else if (atom=='#') {
            //1//if comment
            if (w_size) {
                //2//if previous not appended
                Tnode new=tok_node(w_start, w_size, word_type(&p->source, w_start, w_size), row, get_column(column, w_size) );
                
                if (!t_push(&p->tnode, new) ) {
                    //3//token push
                    printf("exception: pushing 'tnode'\n");
                    return ierr;
                }
                w_size=0;//reset size
            }
            int next;
            while ((next=fgetc(pf) )!=EOF && (char)next!='\n') {
                //2//filter everything until EOF or \n
            }
            if (next==EOF) {
                break;//2//
            } else if (check_last_toktype(&p->tnode, TT_ENDLN) ) {
                s_pull(&p->source);//2//if precious char was a newline then remove it
                --pos;//restore the unnecessary incremented pos
            } else {
                //2//if previous token was newline
                p->source.ptr[p->source.size-2]=(char)next;
                Tnode new=tok_node(pos, 1, TT_ENDLN, row, column);
                
                if (!t_push(&p->tnode, new) ) {
                    //3//token push
                    printf("exception: pushing 'tnode'\n");
                    return ierr;
                }
            }
            column=0;//2//reset column
            ++row;//update row
        } else if (s_member_of(s_a_zA_Z_, atom) || s_member_of(s_0_9, atom) || s_member_of("\"'", atom) ){
            //1//if word/number/_/"/'
            if (!w_size) {
                //2//if it's a new word
                w_start=pos;//set word starting pos
                if (s_member_of("'\"", atom) ) {
                    //3//if it's a opening quote
                    quote=atom;//set the quote
                } 
            } else if (w_size==1 && (s_member_of(s_a_zA_Z_, p->source.ptr[w_start + w_size]) && !s_member_of(s_a_zA_Z_, p->source.ptr[w_start]) ) ) {
                printf("%s:%zu:%zu: error:\n      invalid syntax\n        %c...\n", p->filename, row, column, p->source.ptr[pos]);
                return ierr;
            }
            ++w_size;//inc word size
        } else {
            //1//else error
            printf("%s:%zu:%zu: error:\n        '%c'\n", p->filename, row, column, atom);
            return ierr;
        }
        //column update
        ++pos;//every pos update
    }
    if (quote!='\0') {
        //0//if quote not terminated
        printf("%s:%zu:%zu: error:\n       ", p->filename, row, column);
        //t_print(p.source, w_start, w_size);
        printf("\n");
        return ierr;
    } else if (w_size) {
        //0//if previous word not appended
        Tnode new=tok_node(w_start, w_size, word_type(&p->source, w_start, w_size), row, get_column(column, w_size) );
        
        if (!t_push(&p->tnode, new) ) {
            //1//token push
            printf("error pushing for 'tnode'\n");
            return ierr;
        }
    } else if (check_last_toktype(&p->tnode, TT_ENDLN) ) {
        //0//if last token was newline → remove it from tnodes and source
        t_pull(&p->tnode);
        s_pull(&p->source);
    }
    fclose(pf);//close file
    return true;
};