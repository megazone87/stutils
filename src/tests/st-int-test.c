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

#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "st_rand.h"
#include "st_int.h"

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

static int unit_test_parse_wt_int_array()
{
#define N_ARR 8
    char line[1024];
    char buf[64];
    st_wt_int_t ref[N_ARR];
    float def_wt = 0.1234;
    st_wt_int_t *a = NULL;
    int n = 0;
    int i;
    int ncase;

    fprintf(stderr, " Testing st_parse_wt_int_array...\n");

    srand((unsigned int)time(NULL));
    for (i = 0; i < N_ARR; i++) {
        ref[i].i = rand() % 19 + (-9);
        if (i == 0) {
            ref[i].w = 2;
        } else {
            ref[i].w = (float)st_random(-10.0, 10.0);
        }
#ifdef _ST_TEST_DEBUG_
        printf("%d:%f\n", ref[i].i, ref[i].w);
#endif
    }

    ncase = 1;
    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    line[0] = '\0';
    for (i = 0; i < N_ARR - 1; i++) {
        snprintf(buf, 64, "%d,", ref[i].i);
        strcat(line, buf);
    }
    snprintf(buf, 64, "%d", ref[i].i);
    strcat(line, buf);
#ifdef _ST_TEST_DEBUG_
    printf("%s\n", line);
#endif

    if(st_parse_wt_int_array(line, &a, &n, def_wt) != 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    if (n != N_ARR) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    for (i = 0; i < N_ARR; i++) {
#ifdef _ST_TEST_DEBUG_
        printf("%d:%f\n", a[i].i, a[i].w);
#endif
        if (a[i].i != ref[i].i) {
            fprintf(stderr, "Failed\n");
            goto FAILED;
        }
        if (!APPROX_EQUAL(a[i].w, def_wt)) {
            fprintf(stderr, "Failed\n");
            goto FAILED;
        }
    }
    safe_free(a);
    n = 0;
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    line[0] = '\0';
    for (i = 0; i < N_ARR - 1; i++) {
        snprintf(buf, 64, "%d:%g,", ref[i].i, ref[i].w);
        strcat(line, buf);
    }
    snprintf(buf, 64, "%d:%g", ref[i].i, ref[i].w);
    strcat(line, buf);
#ifdef _ST_TEST_DEBUG_
    printf("%s\n", line);
#endif

    if(st_parse_wt_int_array(line, &a, &n, def_wt) != 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    if (n != N_ARR) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    for (i = 0; i < N_ARR; i++) {
#ifdef _ST_TEST_DEBUG_
        printf("%d:%f\n", a[i].i, a[i].w);
#endif
        if (a[i].i != ref[i].i) {
            fprintf(stderr, "Failed\n");
            goto FAILED;
        }
        if (!APPROX_EQUAL(a[i].w, ref[i].w)) {
            fprintf(stderr, "Failed\n");
            goto FAILED;
        }
    }
    safe_free(a);
    n = 0;
    fprintf(stderr, "Passed\n");

    return 0;

FAILED:
    safe_free(a);
    return -1;
}

static int run_all_tests()
{
    int ret = 0;

    if (unit_test_parse_int_array() != 0) {
        ret = -1;
    }

    if (unit_test_parse_wt_int_array() != 0) {
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
