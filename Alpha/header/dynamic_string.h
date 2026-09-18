#ifndef DYNAMIC_STRING_H
#define DYNAMIC_STRING_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

extern char *s_a_zA_Z;
extern char *s_a_zA_Z_;
extern char *s_0_9;

typedef enum {
    SS_NO_ERR,
    SS_INDEX_ERR,
    SS_PUSH_ERR,
    SS_PULL_ERR,
    SS_LOAD_ERR,
} STRING_STATUS;


typedef struct {
    char *ptr;
    size_t size; // main size is size-1==string terminator. valid access 0 -> size-2
    size_t capacity;
} String;


typedef struct {
    char val;
    bool success;
} S_Index_res;

typedef struct {
    char val;
    bool success;
} S_Value_res;



String s_open(void);

void s_close(String *array);

STRING_STATUS s_push(String *array, char c);

STRING_STATUS s_pull(String *array);

S_Value_res s_get(String *array, size_t index);

size_t s_index(String *array, char target);

size_t s_merge(String *a, String *b);

bool s_member_of(char *string, char target);

bool s_eq(char *a, char *b);

#endif //DYNAMIC_STRING_H