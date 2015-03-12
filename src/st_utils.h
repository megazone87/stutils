#ifndef  _ST_UTILS_H_
#define  _ST_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "st_macro.h"

long make_long(unsigned int a, unsigned int b);

void split_long(long l, unsigned int *a, unsigned int *b);

void remove_newline(char *line);

void remove_leading_space(char *line);

int split_line(char *line, char fields[][MAX_LINE_LEN], int max_field,
    char *sep);

unsigned int highest_bit_mask(unsigned int num, int overflow);

void iqsort(int *arr, int l, int u,
        int cmp(int *arr, int i, int j, void *args), void *args);

typedef enum _encoding_type_t_
{
    ENCODING_GBK  = 0,
    ENCODING_UTF8 = 1,
} encoding_type_t;

int get_next_char(const char *token, encoding_type_t encoding);

char* get_next_token(char *line, char *token);

void remove_newline(char *line);

uint32_t MurmurHash2 ( const void * key, int len, uint32_t seed );

#ifdef __cplusplus
}
#endif

#endif

