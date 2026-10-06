#ifndef DYNAMIC_STRING_H
#define DYNAMIC_STRING_H


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>


extern const char *const s_a_zA_Z;
extern const char *const s_a_zA_Z_;
extern const char *const s_0_9;

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


String s_open(void);

void s_close(String *array);

STRING_STATUS s_push(String *array, char c);

STRING_STATUS s_pull(String *array);

const char *const s_get(String *s, const size_t index);

bool s_member_of(const char *const string, const char target);

bool s_eq(const char *const a, const char *const b);

bool s_has_only_from(char *a, const char *const b);

#endif //DYNAMIC_STRING_H