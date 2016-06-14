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

#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "st_utils.h"

int ref[24][4] = {
    {1, 2, 3, 4},
    {1, 2, 4, 3},
    {1, 3, 2, 4},
    {1, 3, 4, 2},
    {1, 4, 3, 2},
    {1, 4, 2, 3},
    {2, 1, 3, 4},
    {2, 1, 4, 3},
    {2, 3, 1, 4},
    {2, 3, 4, 1},
    {2, 4, 3, 1},
    {2, 4, 1, 3},
    {3, 2, 1, 4},
    {3, 2, 4, 1},
    {3, 1, 2, 4},
    {3, 1, 4, 2},
    {3, 4, 1, 2},
    {3, 4, 2, 1},
    {4, 2, 3, 1},
    {4, 2, 1, 3},
    {4, 3, 2, 1},
    {4, 3, 1, 2},
    {4, 1, 3, 2},
    {4, 1, 2, 3},
};

int res[24][4];
int num_res;

static int save_int(void *base, size_t n, void *args)
{
    int *A;
    size_t i;

    if (n != 4) {
        fprintf(stderr, "num not match");
        return -1;
    }

    A = (int *)base;
    for (i = 0; i < n; i++) {
        res[num_res][i] = A[i];
    }
    num_res++;

    return 0;
}

static int unit_test_permutation()
{
    int A[4] = {1, 2, 3, 4};
    size_t n = 4;

    int ncase = 0;

    int i;
    int j;

    fprintf(stderr, "  Testing permutation...\n");
    fprintf(stderr, "    Case %d...", ncase++);
    num_res = 0;
    if (st_permutation((void *)A, n, sizeof(A[0]), save_int, NULL) < 0) {
        fprintf(stderr, "Failed.\n");
        return -1;
    }

    for (i = 0; i < 24; i++) {
        for (j = 0; j < 4; j++) {
            if (res[i][j] != ref[i][j]) {
                fprintf(stderr, "Failed.\n");
                return -1;
            }
        }
    }
    fprintf(stderr, "Passed.\n");

    return 0;
}

static int check_tok(const char *line, const char *sep,
        const char *str, int n, int field_len)
{
    char buf[MAX_LINE_LEN];
    char *word, *brk;
    int i;

    strcpy(buf, line);
    for (word = strtok_r(buf, sep, &brk), i = 0;
         word && i < n;
         word = strtok_r(NULL, sep, &brk), i++) {
        if (strcmp(word, str + i*field_len) != 0) {
            fprintf(stderr, "token[%d] not match. [%s/%s]\n",
                    i, word, str + i*field_len);
            return -1;
        }
    }
    if (i != n || word != NULL) {
        fprintf(stderr, "Size not match.\n");
        return -1;
    }

    return 0;
}

static int unit_test_string()
{
    char line[MAX_LINE_LEN];
    char sep[MAX_LINE_LEN];
    char *str = NULL;
    int n_field = 4;
    int field_len = 5;

    int ncase = 0;

    int n;

    str = malloc(n_field * field_len);
    if(str == NULL) {
        goto ERR;
    }

    fprintf(stderr, "  Testing permutation...\n");
    /**********************************************/
    /**********************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(line, "fooba haha");
    strcpy(sep, " ");
    n = split_line(line, str, n_field, field_len, sep);
    if (n >= 0) {
        fprintf(stderr, "Failed.\n");
        goto ERR;
    }
    fprintf(stderr, "Passed.\n");

    /**********************************************/
    /**********************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(line, "foo bar\tha\ti ha");
    strcpy(sep, " \t");
    n = split_line(line, str, n_field, field_len, sep);
    if (n >= 0) {
        fprintf(stderr, "Failed.\n");
        goto ERR;
    }
    fprintf(stderr, "Passed.\n");

    /**********************************************/
    /**********************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(line, " \t \t");
    strcpy(sep, " \t");
    n = split_line(line, str, n_field, field_len, sep);
    if (n != 0) {
        fprintf(stderr, "Failed.\n");
        goto ERR;
    }
    fprintf(stderr, "Passed.\n");

    /**********************************************/
    /**********************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(line, "foo");
    strcpy(sep, " \t");
    n = split_line(line, str, n_field, field_len, sep);
    if (n < 0) {
        fprintf(stderr, "Failed.\n");
        goto ERR;
    }
    if (check_tok(line, sep, str, n, field_len) < 0) {
        fprintf(stderr, "Failed.\n");
        goto ERR;
    }
    fprintf(stderr, "Passed.\n");

    /**********************************************/
    /**********************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(line, "foo   \t  ");
    strcpy(sep, " \t");
    n = split_line(line, str, n_field, field_len, sep);
    if (n < 0) {
        fprintf(stderr, "Failed.\n");
        goto ERR;
    }
    if (check_tok(line, sep, str, n, field_len) < 0) {
        fprintf(stderr, "Failed.\n");
        goto ERR;
    }
    fprintf(stderr, "Passed.\n");

    /**********************************************/
    /**********************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(line, "fo b");
    strcpy(sep, "");
    n = split_line(line, str, n_field, field_len, sep);
    if (n < 0) {
        fprintf(stderr, "Failed.\n");
        goto ERR;
    }
    if (check_tok(line, sep, str, n, field_len) < 0) {
        fprintf(stderr, "Failed.\n");
        goto ERR;
    }
    fprintf(stderr, "Passed.\n");
    /**********************************************/
    /**********************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(line, "f=oo =bar");
    strcpy(sep, " \t=");
    n = split_line(line, str, n_field, field_len, sep);
    if (n < 0) {
        fprintf(stderr, "Failed.\n");
        goto ERR;
    }
    if (check_tok(line, sep, str, n, field_len) < 0) {
        fprintf(stderr, "Failed.\n");
        goto ERR;
    }
    fprintf(stderr, "Passed.\n");

    /**********************************************/
    /**********************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(line, " f=oo =bar\t");
    strcpy(sep, " \t=");
    n = split_line(line, str, n_field, field_len, sep);
    if (n < 0) {
        fprintf(stderr, "Failed.\n");
        goto ERR;
    }
    if (check_tok(line, sep, str, n, field_len) < 0) {
        fprintf(stderr, "Failed.\n");
        goto ERR;
    }
    fprintf(stderr, "Passed.\n");
    safe_free(str);
    return 0;

ERR:
    safe_free(str);
    return -1;
}

static int unit_test_parse_int_array()
{
#define N_ARR 8
    char line[1024];
    char buf[8];
    int ref[N_ARR];
    int *a = NULL;
    int n = 0;
    int i;
    int ncase;

    fprintf(stderr, " Testing st_parse_int_array...\n");

    srand((unsigned int)time(NULL));
    for (i = 0; i < N_ARR; i++) {
        ref[i] = rand() % 19 + (-9);
#ifdef _ST_TEST_DEBUG_
        printf("%d\n", ref[i]);
#endif
    }

    ncase = 1;
    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    line[0] = '\0';
    for (i = 0; i < N_ARR - 1; i++) {
        snprintf(buf, 8, "%d,", ref[i]);
        strcat(line, buf);
    }
    snprintf(buf, 8, "%d", ref[i]);
    strcat(line, buf);
#ifdef _ST_TEST_DEBUG_
    printf("%s\n", line);
#endif

    if(st_parse_int_array(line, &a, &n) != 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    if (n != N_ARR) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    for (i = 0; i < N_ARR; i++) {
#ifdef _ST_TEST_DEBUG_
        printf("%d\n", a[i]);
#endif
        if (a[i] != ref[i]) {
            fprintf(stderr, "Failed\n");
            goto FAILED;
        }
    }
    safe_free(a);
    n = 0;
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    snprintf(line, 1024, "%d", ref[0]);
#ifdef _ST_TEST_DEBUG_
    printf("%s\n", line);
#endif

    if(st_parse_int_array(line, &a, &n) != 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    if (n != 1) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
#ifdef _ST_TEST_DEBUG_
    printf("%d\n", a[0]);
#endif
    if (a[0] != ref[0]) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    safe_free(a);
    n = 0;
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(line, "f,1,2,df,-1");
#ifdef _ST_TEST_DEBUG_
    printf("%s\n", line);
#endif

    if(st_parse_int_array(line, &a, &n) == 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    safe_free(a);
    n = 0;
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(line, ",1,2,0,-1");
#ifdef _ST_TEST_DEBUG_
    printf("%s\n", line);
#endif

    if(st_parse_int_array(line, &a, &n) == 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    safe_free(a);
    n = 0;
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(line, ",1,,2,0,-1");
#ifdef _ST_TEST_DEBUG_
    printf("%s\n", line);
#endif

    if(st_parse_int_array(line, &a, &n) == 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    safe_free(a);
    n = 0;
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(line, "1,-1,");
#ifdef _ST_TEST_DEBUG_
    printf("%s\n", line);
#endif

    if(st_parse_int_array(line, &a, &n) == 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    safe_free(a);
    n = 0;
    fprintf(stderr, "Passed\n");

    return 0;

FAILED:
    safe_free(a);
    return -1;
}

static int unit_test_str_replace()
{
#define FROM "${x}"
#define TO "hello"
    char res[256];
    char src[256];
    int ncase;

    fprintf(stderr, " Testing st_str_replace...\n");

    ncase = 1;
    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(src, "xxxxx");

    if(st_str_replace(res, 256, src, FROM, TO, 0) != 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    if (strcmp(res, src) != 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
#ifdef _ST_TEST_DEBUG_
    printf("%s\n", res);
#endif
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(src, FROM"xxxx");

    if(st_str_replace(res, 256, src, FROM, TO, 0) != 1) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    if (strcmp(res, TO"xxxx") != 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
#ifdef _ST_TEST_DEBUG_
    printf("%s\n", res);
#endif
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(src, "xx"FROM"xx");

    if(st_str_replace(res, 256, src, FROM, TO, 0) != 1) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    if (strcmp(res, "xx"TO"xx") != 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
#ifdef _ST_TEST_DEBUG_
    printf("%s\n", res);
#endif
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(src, "xxxx"FROM);

    if(st_str_replace(res, 256, src, FROM, TO, 0) != 1) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    if (strcmp(res, "xxxx"TO) != 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
#ifdef _ST_TEST_DEBUG_
    printf("%s\n", res);
#endif
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(src, FROM"xxxx"FROM);

    if(st_str_replace(res, 256, src, FROM, TO, 0) != 2) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    if (strcmp(res, TO"xxxx"TO) != 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
#ifdef _ST_TEST_DEBUG_
    printf("%s\n", res);
#endif
    fprintf(stderr, "Passed\n");
    return 0;

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(src, FROM"xxxx"FROM);

    if(st_str_replace(res, 256, src, FROM, TO, 1) != 1) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    if (strcmp(res, TO"xxxx"FROM) != 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
#ifdef _ST_TEST_DEBUG_
    printf("%s\n", res);
#endif
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(src, FROM"xxxx"FROM);

    if(st_str_replace(res, 256, src, FROM, TO, 3) != 2) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    if (strcmp(res, TO"xxxx"TO) != 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
#ifdef _ST_TEST_DEBUG_
    printf("%s\n", res);
#endif
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(src, FROM"xxxx"FROM);

    if(st_str_replace(res, 256, src, FROM, NULL, 0) != 2) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    if (strcmp(res, "xxxx") != 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
#ifdef _ST_TEST_DEBUG_
    printf("%s\n", res);
#endif
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(src, FROM"xxxx"FROM);

    if(st_str_replace(res, 256, src, FROM, "", 0) != 2) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    if (strcmp(res, "xxxx") != 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
#ifdef _ST_TEST_DEBUG_
    printf("%s\n", res);
#endif
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(src, FROM"xxxx"FROM);

    if(st_str_replace(res, strlen(TO) + 4, src, FROM, TO, 0) >= 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
#ifdef _ST_TEST_DEBUG_
    printf("%s\n", res);
#endif
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(src, FROM"xxxx"FROM);

    if(st_str_replace(res, 2*strlen(TO) + 4, src, FROM, TO, 0) >= 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
#ifdef _ST_TEST_DEBUG_
    printf("%s\n", res);
#endif
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(src, FROM"xxxx"FROM);

    if(st_str_replace(res, strlen(TO) - 1, src, FROM, TO, 0) >= 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
#ifdef _ST_TEST_DEBUG_
    printf("%s\n", res);
#endif
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(src, FROM"xxxx"FROM);

    if(st_str_replace(res, 5, src, FROM, NULL, 0) != 2) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    if (strcmp(res, "xxxx") != 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
#ifdef _ST_TEST_DEBUG_
    printf("%s\n", res);
#endif
    fprintf(stderr, "Passed\n");

    return 0;

FAILED:
    return -1;
}

#define MAX_STR_LEN 256
static int unit_test_str2ll()
{
    char num[MAX_STR_LEN];
    char suf[MAX_STR_LEN];
    long long ref = 2568340;
    int ncase;

    fprintf(stderr, " Testing st_str2ll...\n");

    ncase = 1;
    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(num, "xx12");

    if(st_str2ll(num) != 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    snprintf(num, MAX_STR_LEN, "%lld", ref);

    if(st_str2ll(num) != ref) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(suf, "Ki");
    snprintf(num, MAX_STR_LEN, "%lld%s", ref, suf);

    if(st_str2ll(num) != ref * 1024) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(suf, "Kixx");
    snprintf(num, MAX_STR_LEN, "%lld%s", ref, suf);

    if(st_str2ll(num) != ref) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(suf, "Mi");
    snprintf(num, MAX_STR_LEN, "%lld%s", ref, suf);

    if(st_str2ll(num) != ref * 1024 * 1024) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(suf, "Gi");
    snprintf(num, MAX_STR_LEN, "%lld%s", ref, suf);

    if(st_str2ll(num) != ref * 1024 * 1024 * 1024) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(suf, "k");
    snprintf(num, MAX_STR_LEN, "%lld%s", ref, suf);

    if(st_str2ll(num) != ref * 1000) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(suf, "kxx");
    snprintf(num, MAX_STR_LEN, "%lld%s", ref, suf);

    if(st_str2ll(num) != ref) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(suf, "M");
    snprintf(num, MAX_STR_LEN, "%lld%s", ref, suf);

    if(st_str2ll(num) != ref * 1000 * 1000) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(suf, "G");
    snprintf(num, MAX_STR_LEN, "%lld%s", ref, suf);

    if(st_str2ll(num) != ref * 1000 * 1000 * 1000) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    fprintf(stderr, "Passed\n");

    return 0;

FAILED:
    return -1;
}

static int unit_test_ll2str()
{
    char num[MAX_STR_LEN];
    char suf[MAX_STR_LEN];
    char buf[MAX_STR_LEN];
    long long ref = 2568341;
    int ncase;

    fprintf(stderr, " Testing st_ll2str...\n");

    ncase = 1;
    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    if(st_ll2str(buf, 9, ref * 1024, true) != NULL) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    snprintf(num, MAX_STR_LEN, "%lld", ref);

    if(strcmp(st_ll2str(buf, MAX_STR_LEN, ref, true), num) != 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(suf, "Ki");
    snprintf(num, MAX_STR_LEN, "%lld%s", ref, suf);

    if(strcmp(st_ll2str(buf, MAX_STR_LEN, ref * 1024, true), num) != 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(suf, "Mi");
    snprintf(num, MAX_STR_LEN, "%lld%s", ref, suf);

    if(strcmp(st_ll2str(buf, MAX_STR_LEN,
                    ref * 1024 * 1024, true), num) != 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    if(st_ll2str(buf, 8, ref * 1000, false) != NULL) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    snprintf(num, MAX_STR_LEN, "%lld", ref);

    if(strcmp(st_ll2str(buf, MAX_STR_LEN, ref, false), num) != 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(suf, "k");
    snprintf(num, MAX_STR_LEN, "%lld%s", ref, suf);

    if(strcmp(st_ll2str(buf, MAX_STR_LEN, ref * 1000, false), num) != 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    strcpy(suf, "M");
    snprintf(num, MAX_STR_LEN, "%lld%s", ref, suf);

    if(strcmp(st_ll2str(buf, MAX_STR_LEN,
                    ref * 1000 * 1000, false), num) != 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    fprintf(stderr, "Passed\n");

    return 0;

FAILED:
    return -1;
}

static int run_all_tests()
{
    int ret = 0;

    if (unit_test_permutation() != 0) {
        ret = -1;
    }

    if (unit_test_string() != 0) {
        ret = -1;
    }

    if (unit_test_parse_int_array() != 0) {
        ret = -1;
    }

    if (unit_test_str_replace() != 0) {
        ret = -1;
    }

    if (unit_test_str2ll() != 0) {
        ret = -1;
    }

    if (unit_test_ll2str() != 0) {
        ret = -1;
    }

    return ret;
}

int main(int argc, const char *argv[])
{
    int ret;

    fprintf(stderr, "Start testing...\n");
    ret = run_all_tests();
    if (ret != 0) {
        fprintf(stderr, "Tests failed.\n");
    } else {
        fprintf(stderr, "Tests succeeded.\n");
    }

    return ret;
}
