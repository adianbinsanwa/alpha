#include "../header/lexer.h"

//#include <errno.h> #include <string.h>

/* 
 * incomplete stuff:
 *     column at :- lexer(),
 *     proper word checking at :- word_type(),
 */

static bool char_scanner(FILE *pf, Package *p);


static TnodeType
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
             return TT_END;
    }
};



static TnodeType
word_type(String *s, const size_t pos, const size_t width, const size_t row, const size_t column) {
    char word[width];
    word[width]='\0';
    
    for (size_t i=0; i < width; ++i) {
        word[i]=*s_get(s, pos + i);
    }
    
    if (s_member_of(s_a_zA_Z_, word[0]) ) {
        //0//
        return TT_WORD;
    } else if (width > 2 && word[0]=='0' && s_member_of("xb", word[1]) ) {
        //0//
        return word[1]=='x' ? TT_HEX : TT_BIN;
    } else if (s_has_only_from(word, s_0_9) ) {
        //0//
        return TT_NUM;
    } else {
        //0//
        printf("error: lexer: %s, row: %zu, col: %zu\n%s!\n", word[0]=='\0' ? "null" : word, pos, width, s->ptr);
        return TT_END;
    }
    
    return TT_NUM;
};


static inline size_t
get_column(const size_t a, const size_t b) {
    return a - b;
};


static inline bool
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
    bool ret= char_scanner(pfile, p);//tokenize
    
    fclose(pfile);//close file
    return ret;
};

//############-tokenizer-#############//

static bool
char_scanner(FILE *pf, Package *p) {
    /*load file and manage line ends
     *split words/chars and pack tokens
     *pos tracks the normalized source
     *column/row track the original input
    */
    //system
    
    size_t row=1, column=0, pos=0;//row count//column count//current pos throughout the whole file string
    
    //helpers
    char quote='\0';//quote holder indicating in quotes + which quote "=2/'=1/none=0
    size_t w_size=0, w_start= 0;//counts the size of the word //holds the start pos of a word
    
    //error to return when something internal goes wrong
    bool ierr=false;
                
    if (!t_push(&p->tnode, tok_node(0, 0, TT_SCOPE, 0, 0) ) ) {
        //0//token push
        printf("exception: pushing 'tnode'\n");
        return ierr;
    }
    
    
    int8_t a= 0;
    while ((a=fgetc(pf) ) != EOF) {
        //0//
        char atom=(char)a;//cast int a to char
        
        if (s_push(&p->source, atom)!=SS_NO_ERR) {
            //1//source push
            printf("exception: pushing 'source'\n");
            return ierr;
        } else if (quote!='\0') {
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
                ++column;//aggresive update
                ++w_size;
                ++pos;
            } else if (atom=='\n') {
                printf("error: ");
                return ierr;
            }
        } else if (s_member_of(" \n(){},", atom) ) {
            //1//if valid symbols
            Tnode *temp;
            
            if (w_size) {
                //2//if previous not appended
                size_t col= get_column(column, w_size);
                
                Tnode new=tok_node(w_start, w_size, word_type(&p->source, w_start, w_size, row, col), row, col);
                if (new.type==TT_END ) {
                    //3//
                    return ierr;
                } else if (!t_push(&p->tnode, new) ) {
                    //3//token push
                    printf("exception: pushing 'tnode'\n");
                    return ierr;
                }
                w_size=0;//reset size
            }
            
            if (atom==',' && check_last_toktype(&p->tnode, TT_COMMA) ) {
                //2//if an newline already appended
                printf("error");
                return ierr;
            } else if (atom!=' ' && (atom!='\n' || ( (temp= t_get(&p->tnode, p->tnode.size - 2) )->type!=TT_ENDLN && temp->type!=TT_SCOPE ) ) ) {
                //2//
                Tnode new=tok_node(pos, 1, symbol_type(atom), row, column);
                    
                if (!t_push(&p->tnode, new) ) {
                    //3//token push
                    printf("exception: pushing 'tnode'\n");
                    return ierr;
                }
            }
            if (atom=='\n') {
                column=0;//2//reset column
                ++row;//update row
            }
        } else if (atom=='#') {
            //1//if comment
            if (w_size) {
                //2//if previous not appended
                size_t col= get_column(column, w_size);
                
                Tnode new=tok_node(w_start, w_size, word_type(&p->source, w_start, w_size, row, col), row, col);
                if (new.type==TT_END ) {
                    //3//
                    return ierr;
                } else if (!t_push(&p->tnode, new) ) {
                    //3//token push
                    printf("exception: pushing 'tnode'\n");
                    return ierr;
                }
                w_size=0;//reset size
            }
            s_pull(&p->source);
            
            int8_t next;
            while ((next=fgetc(pf) )!=EOF && (char)next!='\n') {
                //2//filter everything until EOF or \n
                ++column;
            }
            
            Tnode *temp;
            if (next != EOF && (temp= t_get(&p->tnode, p->tnode.size - 2) )->type!=TT_ENDLN && temp->type!=TT_SCOPE) {
                //2//if previous token was newline
                p->source.ptr[p->source.size-2]= (char)next;
                Tnode new=tok_node(pos, 1, TT_ENDLN, row, column);
                
                if (!t_push(&p->tnode, new) ) {
                    //3//token push
                    printf("exception: pushing 'tnode'\n");
                    return ierr;
                }
            } else {
                //2//
                --pos;
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
            }
            ++w_size;//inc word size
        } else {
            //1//else error
            printf("%s:%zu:%zu: error:\n        '%c'\n", p->filename, row, column, atom);
            return ierr;
        }
        ++column;//column update
        ++pos;//every pos update
    }
    if (quote!='\0') {
        //0//if quote not terminated
        printf("%s:%zu:%zu: error:\n       ", p->filename, row, column);
        printf("\n");
        return ierr;
    } else if (w_size) {
        //0//if previous word not appended
        size_t col= get_column(column, w_size);
        
        Tnode new=tok_node(w_start, w_size, word_type(&p->source, w_start, w_size, row, col), row, col);
        if (new.type==TT_END ) {
            //1//
            return ierr;
        } else if (!t_push(&p->tnode, new) ) {
            //1//token push
            printf("error pushing for 'tnode'\n");
            return ierr;
        }
    } else if (check_last_toktype(&p->tnode, TT_COMMA) ) {
        //0//if last token was newline → remove it from tnodes and source
        printf("error: invalid ','");
        return false;
    } else if (t_get(&p->tnode, p->tnode.size - 2)->type==TT_ENDLN) {
        //0//
        t_pull(&p->tnode);
    }
    return true;
};