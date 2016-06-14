/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Wang Jian
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef  _ST_UTILS_H_
#define  _ST_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

#include <stutils/st_macro.h>

const char* st_version();

long make_long(unsigned int a, unsigned int b);

void split_long(long l, unsigned int *a, unsigned int *b);

void remove_newline(char *line);

void remove_leading_space(char *line);

void trim(char *line);

int split_line(const char *line, char *fields,
        int n_field, int field_len, const char *seps);

unsigned int highest_bit_mask(unsigned int num, int overflow);

void iqsort(int *arr, int l, int u,
        int cmp(int *arr, int i, int j, void *args), void *args);

typedef enum _encoding_type_t_
{
    ENCODING_GBK  = 0,
    ENCODING_UTF8 = 1,
} encoding_type_t;

int get_next_char(const char *token, encoding_type_t encoding);

const char* get_next_token(const char *line, char *token);

uint32_t MurmurHash2 ( const void * key, int len, uint32_t seed );

/* The largest number rand will return (same as INT_MAX).  */
#define ST_RAND_MAX        2147483647
int st_rand();
double st_random(double min, double max);
int st_rand_r(unsigned int *seed);
void st_srand(unsigned int seed);

void st_shuffle(int *a, size_t n);
void st_shuffle_r(int *a, size_t n, unsigned *rand);

char* st_fgets(char **line, size_t *sz, FILE *fp, bool *err);
int st_readline(FILE *fp, const char *fmt, ...);

int st_escape(const char *str, char *ans, size_t ans_len);

int st_escape_args(int argc, const char *argv[], char *ans, size_t ans_len);

off_t st_fsize(const char *filename);

typedef struct _st_gauss_r_t {
    double mean;
    double stdev;
    unsigned seed;

    double V1;
    double V2;
    double S;
    int phase;
} st_gauss_r_t;

double st_gaussrand();
double st_normrand(double mean, double stdev);

void st_gauss_r_init(st_gauss_r_t *gauss, double mean,
        double stdev, unsigned seed);
double st_gaussrand_r(st_gauss_r_t *gauss);

int st_permutation(void *base, size_t n, size_t sz,
        int (*callback)(void *base, size_t n, void *args), void *args);

void st_qsort(void *const pbase, size_t total_elems, size_t size,
	    int (*cmp) (const void *, const void *, void *), void *arg);

int st_parse_int_array(const char *line, int **arr, int *n_arr);

/**
 * Replace 'max_num' ocurrences of 'from' in 'src' to 'to',
 * storing result to 'res'.
 *
 * @return number of ocurrences replaced.
 */
int st_str_replace(char* res, size_t res_len,
        const char* src, const char* from, const char* to,
        int max_num);

/**
 * Convert string to long long int.
 * Support k,M,G,T... or Ki,Mi,Gi,Ti... suffixes.
 *
 * @param[in] str string format.
 * @return converted number.
 */
long long st_str2ll(const char *str);

/**
 * Convert long long int to string.
 * Support k,M,G,T... or Ki,Mi,Gi,Ti... suffixes.
 *
 * @param[out] str string buffer.
 * @param[in] len length of string buffer.
 * @param[in] l long long int.
 * @param[in] binary base-1024 if true, otherwise base-1000.
 * @return NULL on error, otherwise the string buffer.
 */
char* st_ll2str(char *str, size_t len, long long l, bool binary);

#ifdef __cplusplus
}
#endif

#endif
