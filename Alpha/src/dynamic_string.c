#include "../header/dynamic_string.h"

char *s_a_zA_Z="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
char *s_a_zA_Z_="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
char *s_0_9="0123456789";

//###############################//


String 
s_open(void) {
    return (String){.ptr=NULL, .size=1, .capacity=0};
};


void 
s_close(String *array) {
    if (!array->ptr) {
        return;
    }
    free(array->ptr);
    array->ptr=NULL;
    array->size=1;
};


STRING_STATUS
s_push(String *array, char c) {
    if (!array->capacity) {
        char *temp=realloc(array->ptr, (array->size + 1) * sizeof(char) );
        if (!temp) {
            return SS_PUSH_ERR;
        }
        array->ptr=temp;
    } else {
        --array->capacity;
    }
    array->ptr[array->size-1]=c;
    array->ptr[array->size++]='\0';
    return SS_NO_ERR;
};


STRING_STATUS 
s_pull(String *array) {
    if (!array->ptr) {
        return SS_PULL_ERR;
    }
    array->ptr[--array->size-1]='\0';
    ++array->capacity;
    return SS_NO_ERR;
};


//###################################//


char *
s_get(String *s, size_t index) {
    if (index >= s->size) {
       return &s->ptr[s->size-2];   
    }
    return &s->ptr[index];
};


size_t
s_index(String *array, char target) {
    for (size_t i=0; i < array->size; i++) {
        if (array->ptr[i]== target) {
            return i;
        }
    }
    return array->size;
};


size_t
s_merge(String *a, String *b) {
    for (size_t i=0; i < b->size; i++) {
        if (s_push(a, *s_get(b, i) )==SS_PUSH_ERR) {
            return strlen(b->ptr)-i;
        }
    }
    return 0;
};


size_t
s_raw_merge(String *a, char *b) {
    for (size_t i=0; i < strlen(b); i++) {
        if (s_push(a, b[i])==SS_PUSH_ERR) {
            return strlen(b)-i;
        }
    }
    return 0;
};


bool
s_member_of(char *string, char target) {
    for (size_t i=0; string[i]!='\0'; ++i) {
        if (string[i]==target) {
            return true;
        }
    }
    return false;
}    


bool
s_eq(char *a, char *b) {
    if (strlen(a)!=strlen(b) ) {
        return false;
    }
    for (size_t i=0; i!= strlen(a); ++i) {
        if (a[i]!= b[i]) {
            return false;
        }
    }
    return true;
};


bool
s_has_only_from(char *a, char *b) {
    for (size_t i=0; i < strlen(a); ++i) {
        if (!s_member_of(b, a[i]) ) {
            return false;
        }
    }
    return true;
};