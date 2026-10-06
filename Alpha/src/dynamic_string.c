#include "../header/dynamic_string.h"

const char *const s_a_zA_Z="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char *const s_a_zA_Z_="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
const char *const s_0_9="0123456789";

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
    if (!array->ptr) return SS_PULL_ERR;
    array->ptr[--array->size-1]='\0';
    ++array->capacity;
    return SS_NO_ERR;
};


//###################################//


const char *const
s_get(String *s, const size_t index) {
    return &s->ptr[index >= s->size ? s->size-2 : index];
};


bool
s_member_of(const char *const string, const char target) {
    for (size_t i=0; string[i]!='\0'; ++i) {
        if (string[i]==target) return true;
    }
    return false;
}    


bool
s_eq(const char *const a, const char *const b) {
    if (strlen(a)!=strlen(b) ) return false;
    
    for (size_t i=0; i!= strlen(a); ++i) {
        if (a[i]!= b[i]) return false;
    }
    return true;
};


bool
s_has_only_from(char *a, const char *const b) {
    for (size_t i=0; i < strlen(a); ++i) {
        if (!s_member_of(b, a[i]) ) return false;
    }
    return true;
};