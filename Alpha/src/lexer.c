#include "../header/lexer.h"
#include <unistd.h>
//#include <errno.h> #include <string.h>

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
    
    return tok_check(s, pos, width, s_0_9, false)? TT_NUM : TT_WORD;
};

size_t
get_column(size_t a, size_t b) {
    return a;
};


bool
char_scanner(FILE *pf, Package *pack, char *filename) {
    /*load file and manage line ends
     *split words/chars and pack tokens
     *pos tracks the normalized source
     *column/row track the original input
    */
    //system
    
    size_t pos=0;//current pos throughout the whole file string
    size_t column=0;//flag to indicate if previous char was a newline or not
    size_t row=1;
    //helpers
    char quote='\0';//a quote holder indicating in quotes + which quote "=2/'=1/none=0
    size_t w_size=0;//counts the size of the word
    size_t w_start;//holds the start pos of a word
    
    //error to return when something internal goes wrong
    bool ierr=false;
    
    Package p=*pack;
                
    if (!t_push(p.tnode, t_node(0, 0, TT_SCOPE, 0, 0) ) ) {
        //0//token push
        printf("exception: pushing 'tnode'\n");
        return ierr;
    }
    
    int a;
    while ((a=fgetc(pf) ) != EOF) {
        //0//
        char atom=(char)a;//cast int a to char
        
        if (s_push(p.source, atom)!=SS_NO_ERR) {
            //1//source push
            printf("exception: pushing 'source'\n");
            return ierr;
        }
        if (quote!='\0') {
            //1//if in quotes
            ++w_size;//inc word size
            if (atom==quote) {
                //2//if quote terminated
                Tnode new=t_node(w_start, w_size, atom=='\'' ? TT_CHAR : TT_STRING, row, get_column(column, w_size) );
                if (!t_push(p.tnode, new) ) {
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
                if (s_push(p.source, (char)next)!=SS_NO_ERR) {
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
                Tnode new=t_node(w_start, w_size, word_type(p.source, w_start, w_size), row, get_column(column, w_size) );
                
                if (!t_push(p.tnode, new) ) {
                    //3//token push
                    printf("exception: pushing 'tnode'\n");
                    return ierr;
                }
                w_size=0;//reset size
            }
            if (atom!=' ') {
                //2//allow everything except ' '
                if (atom=='\n' && p.tnode->val[p.tnode->size-2].type==TT_ENDLN) {
                    s_pull(p.source);//3//if an newline already appended
                    --pos;//restore the unnecessary incremented pos
                } else {
                    //3//
                    Tnode new=t_node(pos, 1, symbol_type(atom), row, column);
                
                    if (!t_push(p.tnode, new) ) {
                        //4//token push
                        printf("exception: pushing 'tnode'\n");
                        return ierr;
                    }
                }
            } else {
                s_pull(p.source);//2//if space then remove it
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
                Tnode new=t_node(w_start, w_size, word_type(p.source, w_start, w_size), row, get_column(column, w_size) );
                
                if (!t_push(p.tnode, new) ) {
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
            } else if (p.tnode->val[p.tnode->size-2].type!=TT_ENDLN) {
                //2//if previous token was newline
                p.source->ptr[p.source->size-2]=(char)next;
                Tnode new=t_node(pos, 1, TT_ENDLN, row, column);
                
                if (!t_push(p.tnode, new) ) {
                    //3//token push
                    printf("exception: pushing 'tnode'\n");
                    return ierr;
                }
            } else {
                s_pull(p.source);//2//if precious char was a newline then remove it
                --pos;//restore the unnecessary incremented pos
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
            printf("%s:%zu:%zu: error:\n        '%c'\n", filename, row, column, atom);
            return ierr;
        }
        //column update
        ++pos;//every pos update
    }
    if (quote!='\0') {
        //0//if quote not terminated
        printf("%s:%zu:%zu: error:\n       ", filename, row, column);
        //t_print(p.source, w_start, w_size);
        printf("\n");
        return ierr;
    } else if (w_size) {
        //0//if previous word not appended
        Tnode new=t_node(w_start, w_size, word_type(p.source, w_start, w_size), row, get_column(column, w_size) );
        
        if (!t_push(p.tnode, new) ) {
            //1//token push
            printf("error pushing for 'tnode'\n");
            return ierr;
        }
    }
    fclose(pf);//close file
    return true;
};



bool
lexer(char *filename, Package *pack) {
    //open file
    FILE *pfile= fopen(filename, "r");
    
    if (!pfile) {
        //0//if file not open
        perror(filename);
        return false;
    }
    return char_scanner(pfile, pack, filename);//tokenize
};



/*int main() {
    String v=s_open();
    Tnodes t=t_open();
    bool res=lexer("../newfile.a", (Package){.source=&v, .tnode=&t});
    
    if (res){
        printf("data=%s, size=%zu\n", v.ptr, v.size-1);
        for (size_t i=0; i < t.size-1; ++i) {
            t_print(&v, t.val[i].start, t.val[i].width);
            printf(", type=%s, pos:%zu:%zu, fpos:%zu\n\n", d_toktype(t.val[i].type), t.val[i].row, t.val[i].column, t.val[i].start);
        }
    }
    char *c=getcwd(NULL, 0);
    if (c) {   
        printf("%s", c);
        free(c);
    }
      
    s_close(&v);
    t_close(&t);
    return 0;
};*/

