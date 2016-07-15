/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Wang Jian
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

#include "st_string.h"

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

    fprintf(stderr, "  Testing string...\n");
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

static int unit_test_strncatf()
{
    char *prefix = "ans=";
    char buf[MAX_STR_LEN];
    char ref[MAX_STR_LEN];
    int n;
    int ncase;

    fprintf(stderr, " Testing st_strncatf...\n");

    ncase = 1;
    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    n = 19;
    strcpy(buf, "Output: ");
    snprintf(ref, MAX_STR_LEN, "%s%s%d", buf, prefix, n);
    if(st_strncatf(buf, MAX_STR_LEN, "%s%d", prefix, n) == NULL) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    if (strcmp(buf, ref) != 0) {
        fprintf(stderr, "Failed\n");
#ifdef _ST_TEST_DEBUG_
        printf("REF => %s\n", ref);
        printf("RET => %s\n", buf);
#endif
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

    if (unit_test_string() != 0) {
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

    if (unit_test_strncatf() != 0) {
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
