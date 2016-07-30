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

#include "st_utils.h"
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

#define CAP_SEG 256
typedef struct _int_seg_arg_t_ {
    st_int_seg_t union_seg[CAP_SEG];
    int n;
    int sz;
} int_seg_arg_t;

static int unit_test_int_seg_union_one(void *base, size_t n_seg, void *args)
{
    st_int_seg_t union_seg[256];
    int n;
    int i;

    st_int_seg_t *segs = (st_int_seg_t *)base;
    int_seg_arg_t *ref = (int_seg_arg_t *)args;

    n = 0;
    if (st_int_seg_union(union_seg, CAP_SEG, &n, segs, n_seg, ref->sz) < 0) {
        return -1;
    }
    if (n != ref->n) { return -1; }
    for (i = 0; i < n; i++) {
        if (union_seg[i].s != ref->union_seg[i].s) { return -1; }
        if (union_seg[i].e != ref->union_seg[i].e) { return -1; }
    }

    return 0;
}

static int unit_test_int_seg_union()
{
    int_seg_arg_t ref;
    st_int_seg_t segs[CAP_SEG];
    int n_seg;
    int ncase = 0;

    ref.sz = 1000;

    fprintf(stderr, "  Testing st_int_seg_union...\n");
    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 2;
    segs[0].s = 100; segs[0].n = 200;
    segs[1].s = 800; segs[1].n = 200;
    ref.n = 2;
    ref.union_seg[0].s = 100; ref.union_seg[0].e = 300;
    ref.union_seg[1].s = 800; ref.union_seg[1].e = 1000;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 2;
    segs[0].s = 100; segs[0].n = 200;
    segs[1].s = 800; segs[1].n = 250;
    ref.n = 3;
    ref.union_seg[0].s =   0; ref.union_seg[0].e = 50;
    ref.union_seg[1].s = 100; ref.union_seg[1].e = 300;
    ref.union_seg[2].s = 800; ref.union_seg[2].e = 1000;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 4;
    segs[0].s = 100; segs[0].n = 100;
    segs[1].s = 300; segs[1].n = 100;
    segs[2].s = 500; segs[2].n = 100;
    segs[3].s = 700; segs[3].n = 100;
    ref.n = 4;
    ref.union_seg[0].s = 100; ref.union_seg[0].e = 200;
    ref.union_seg[1].s = 300; ref.union_seg[1].e = 400;
    ref.union_seg[2].s = 500; ref.union_seg[2].e = 600;
    ref.union_seg[3].s = 700; ref.union_seg[3].e = 800;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 5;
    segs[0].s = 100; segs[0].n = 100;
    segs[1].s = 300; segs[1].n = 100;
    segs[2].s = 500; segs[2].n = 100;
    segs[3].s = 700; segs[3].n = 100;
    segs[4].s = 0; segs[4].n = 50;
    ref.n = 5;
    ref.union_seg[0].s =   0; ref.union_seg[0].e = 50;
    ref.union_seg[1].s = 100; ref.union_seg[1].e = 200;
    ref.union_seg[2].s = 300; ref.union_seg[2].e = 400;
    ref.union_seg[3].s = 500; ref.union_seg[3].e = 600;
    ref.union_seg[4].s = 700; ref.union_seg[4].e = 800;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 5;
    segs[0].s = 100; segs[0].n = 100;
    segs[1].s = 300; segs[1].n = 100;
    segs[2].s = 500; segs[2].n = 100;
    segs[3].s = 700; segs[3].n = 100;
    segs[4].s = 0; segs[4].n = 100;
    ref.n = 4;
    ref.union_seg[0].s =   0; ref.union_seg[0].e = 200;
    ref.union_seg[1].s = 300; ref.union_seg[1].e = 400;
    ref.union_seg[2].s = 500; ref.union_seg[2].e = 600;
    ref.union_seg[3].s = 700; ref.union_seg[3].e = 800;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 5;
    segs[0].s = 100; segs[0].n = 100;
    segs[1].s = 300; segs[1].n = 100;
    segs[2].s = 500; segs[2].n = 100;
    segs[3].s = 700; segs[3].n = 100;
    segs[4].s = 0; segs[4].n = 150;
    ref.n = 4;
    ref.union_seg[0].s =   0; ref.union_seg[0].e = 200;
    ref.union_seg[1].s = 300; ref.union_seg[1].e = 400;
    ref.union_seg[2].s = 500; ref.union_seg[2].e = 600;
    ref.union_seg[3].s = 700; ref.union_seg[3].e = 800;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 5;
    segs[0].s = 100; segs[0].n = 100;
    segs[1].s = 300; segs[1].n = 100;
    segs[2].s = 500; segs[2].n = 100;
    segs[3].s = 700; segs[3].n = 100;
    segs[4].s = 0; segs[4].n = 250;
    ref.n = 4;
    ref.union_seg[0].s =   0; ref.union_seg[0].e = 250;
    ref.union_seg[1].s = 300; ref.union_seg[1].e = 400;
    ref.union_seg[2].s = 500; ref.union_seg[2].e = 600;
    ref.union_seg[3].s = 700; ref.union_seg[3].e = 800;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 5;
    segs[0].s = 100; segs[0].n = 100;
    segs[1].s = 300; segs[1].n = 100;
    segs[2].s = 500; segs[2].n = 100;
    segs[3].s = 700; segs[3].n = 100;
    segs[4].s = 0; segs[4].n = 300;
    ref.n = 3;
    ref.union_seg[0].s =   0; ref.union_seg[0].e = 400;
    ref.union_seg[1].s = 500; ref.union_seg[1].e = 600;
    ref.union_seg[2].s = 700; ref.union_seg[2].e = 800;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 5;
    segs[0].s = 100; segs[0].n = 100;
    segs[1].s = 300; segs[1].n = 100;
    segs[2].s = 500; segs[2].n = 100;
    segs[3].s = 700; segs[3].n = 100;
    segs[4].s = 0; segs[4].n = 450;
    ref.n = 3;
    ref.union_seg[0].s =   0; ref.union_seg[0].e = 450;
    ref.union_seg[1].s = 500; ref.union_seg[1].e = 600;
    ref.union_seg[2].s = 700; ref.union_seg[2].e = 800;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 5;
    segs[0].s = 100; segs[0].n = 100;
    segs[1].s = 300; segs[1].n = 100;
    segs[2].s = 500; segs[2].n = 100;
    segs[3].s = 700; segs[3].n = 100;
    segs[4].s = 0; segs[4].n = 500;
    ref.n = 2;
    ref.union_seg[0].s =   0; ref.union_seg[0].e = 600;
    ref.union_seg[1].s = 700; ref.union_seg[1].e = 800;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 5;
    segs[0].s = 100; segs[0].n = 100;
    segs[1].s = 300; segs[1].n = 100;
    segs[2].s = 500; segs[2].n = 100;
    segs[3].s = 700; segs[3].n = 100;
    segs[4].s = 0; segs[4].n = 550;
    ref.n = 2;
    ref.union_seg[0].s =   0; ref.union_seg[0].e = 600;
    ref.union_seg[1].s = 700; ref.union_seg[1].e = 800;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 5;
    segs[0].s = 100; segs[0].n = 100;
    segs[1].s = 300; segs[1].n = 100;
    segs[2].s = 500; segs[2].n = 100;
    segs[3].s = 700; segs[3].n = 100;
    segs[4].s = 0; segs[4].n = 650;
    ref.n = 2;
    ref.union_seg[0].s =   0; ref.union_seg[0].e = 650;
    ref.union_seg[1].s = 700; ref.union_seg[1].e = 800;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 5;
    segs[0].s = 100; segs[0].n = 100;
    segs[1].s = 300; segs[1].n = 100;
    segs[2].s = 500; segs[2].n = 100;
    segs[3].s = 700; segs[3].n = 100;
    segs[4].s = 0; segs[4].n = 700;
    ref.n = 1;
    ref.union_seg[0].s =   0; ref.union_seg[0].e = 800;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 5;
    segs[0].s = 100; segs[0].n = 100;
    segs[1].s = 300; segs[1].n = 100;
    segs[2].s = 500; segs[2].n = 100;
    segs[3].s = 700; segs[3].n = 100;
    segs[4].s = 0; segs[4].n = 750;
    ref.n = 1;
    ref.union_seg[0].s =   0; ref.union_seg[0].e = 800;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 5;
    segs[0].s = 100; segs[0].n = 100;
    segs[1].s = 300; segs[1].n = 100;
    segs[2].s = 500; segs[2].n = 100;
    segs[3].s = 700; segs[3].n = 100;
    segs[4].s = 0; segs[4].n = 850;
    ref.n = 1;
    ref.union_seg[0].s =   0; ref.union_seg[0].e = 850;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 5;
    segs[0].s = 100; segs[0].n = 100;
    segs[1].s = 300; segs[1].n = 100;
    segs[2].s = 500; segs[2].n = 100;
    segs[3].s = 700; segs[3].n = 100;
    segs[4].s = 150; segs[4].n = 50;
    ref.n = 4;
    ref.union_seg[0].s = 100; ref.union_seg[0].e = 200;
    ref.union_seg[1].s = 300; ref.union_seg[1].e = 400;
    ref.union_seg[2].s = 500; ref.union_seg[2].e = 600;
    ref.union_seg[3].s = 700; ref.union_seg[3].e = 800;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 5;
    segs[0].s = 100; segs[0].n = 100;
    segs[1].s = 300; segs[1].n = 100;
    segs[2].s = 500; segs[2].n = 100;
    segs[3].s = 700; segs[3].n = 100;
    segs[4].s = 150; segs[4].n = 100;
    ref.n = 4;
    ref.union_seg[0].s = 100; ref.union_seg[0].e = 250;
    ref.union_seg[1].s = 300; ref.union_seg[1].e = 400;
    ref.union_seg[2].s = 500; ref.union_seg[2].e = 600;
    ref.union_seg[3].s = 700; ref.union_seg[3].e = 800;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 5;
    segs[0].s = 100; segs[0].n = 100;
    segs[1].s = 300; segs[1].n = 100;
    segs[2].s = 500; segs[2].n = 100;
    segs[3].s = 700; segs[3].n = 100;
    segs[4].s = 150; segs[4].n = 200;
    ref.n = 3;
    ref.union_seg[0].s = 100; ref.union_seg[0].e = 400;
    ref.union_seg[1].s = 500; ref.union_seg[1].e = 600;
    ref.union_seg[2].s = 700; ref.union_seg[2].e = 800;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 5;
    segs[0].s = 100; segs[0].n = 100;
    segs[1].s = 300; segs[1].n = 100;
    segs[2].s = 500; segs[2].n = 100;
    segs[3].s = 700; segs[3].n = 100;
    segs[4].s = 150; segs[4].n = 300;
    ref.n = 3;
    ref.union_seg[0].s = 100; ref.union_seg[0].e = 450;
    ref.union_seg[1].s = 500; ref.union_seg[1].e = 600;
    ref.union_seg[2].s = 700; ref.union_seg[2].e = 800;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 5;
    segs[0].s = 100; segs[0].n = 100;
    segs[1].s = 300; segs[1].n = 100;
    segs[2].s = 500; segs[2].n = 100;
    segs[3].s = 700; segs[3].n = 100;
    segs[4].s = 150; segs[4].n = 400;
    ref.n = 2;
    ref.union_seg[0].s = 100; ref.union_seg[0].e = 600;
    ref.union_seg[1].s = 700; ref.union_seg[1].e = 800;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 5;
    segs[0].s = 100; segs[0].n = 100;
    segs[1].s = 300; segs[1].n = 100;
    segs[2].s = 500; segs[2].n = 100;
    segs[3].s = 700; segs[3].n = 100;
    segs[4].s = 150; segs[4].n = 500;
    ref.n = 2;
    ref.union_seg[0].s = 100; ref.union_seg[0].e = 650;
    ref.union_seg[1].s = 700; ref.union_seg[1].e = 800;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 5;
    segs[0].s = 100; segs[0].n = 100;
    segs[1].s = 300; segs[1].n = 100;
    segs[2].s = 500; segs[2].n = 100;
    segs[3].s = 700; segs[3].n = 100;
    segs[4].s = 150; segs[4].n = 600;
    ref.n = 1;
    ref.union_seg[0].s = 100; ref.union_seg[0].e = 800;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 5;
    segs[0].s = 100; segs[0].n = 100;
    segs[1].s = 300; segs[1].n = 100;
    segs[2].s = 500; segs[2].n = 100;
    segs[3].s = 700; segs[3].n = 100;
    segs[4].s = 150; segs[4].n = 700;
    ref.n = 1;
    ref.union_seg[0].s = 100; ref.union_seg[0].e = 850;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 5;
    segs[0].s = 100; segs[0].n = 100;
    segs[1].s = 300; segs[1].n = 100;
    segs[2].s = 500; segs[2].n = 100;
    segs[3].s = 700; segs[3].n = 100;
    segs[4].s = 200; segs[4].n = 50;
    ref.n = 4;
    ref.union_seg[0].s = 100; ref.union_seg[0].e = 250;
    ref.union_seg[1].s = 300; ref.union_seg[1].e = 400;
    ref.union_seg[2].s = 500; ref.union_seg[2].e = 600;
    ref.union_seg[3].s = 700; ref.union_seg[3].e = 800;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 5;
    segs[0].s = 100; segs[0].n = 100;
    segs[1].s = 300; segs[1].n = 100;
    segs[2].s = 500; segs[2].n = 100;
    segs[3].s = 700; segs[3].n = 100;
    segs[4].s = 200; segs[4].n = 100;
    ref.n = 3;
    ref.union_seg[0].s = 100; ref.union_seg[0].e = 400;
    ref.union_seg[1].s = 500; ref.union_seg[1].e = 600;
    ref.union_seg[2].s = 700; ref.union_seg[2].e = 800;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 5;
    segs[0].s = 100; segs[0].n = 100;
    segs[1].s = 300; segs[1].n = 100;
    segs[2].s = 500; segs[2].n = 100;
    segs[3].s = 700; segs[3].n = 100;
    segs[4].s = 200; segs[4].n = 250;
    ref.n = 3;
    ref.union_seg[0].s = 100; ref.union_seg[0].e = 450;
    ref.union_seg[1].s = 500; ref.union_seg[1].e = 600;
    ref.union_seg[2].s = 700; ref.union_seg[2].e = 800;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 5;
    segs[0].s = 100; segs[0].n = 100;
    segs[1].s = 300; segs[1].n = 100;
    segs[2].s = 500; segs[2].n = 100;
    segs[3].s = 700; segs[3].n = 100;
    segs[4].s = 250; segs[4].n = 25;
    ref.n = 5;
    ref.union_seg[0].s = 100; ref.union_seg[0].e = 200;
    ref.union_seg[1].s = 250; ref.union_seg[1].e = 275;
    ref.union_seg[2].s = 300; ref.union_seg[2].e = 400;
    ref.union_seg[3].s = 500; ref.union_seg[3].e = 600;
    ref.union_seg[4].s = 700; ref.union_seg[4].e = 800;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    n_seg = 5;
    segs[0].s = 100; segs[0].n = 100;
    segs[1].s = 300; segs[1].n = 100;
    segs[2].s = 500; segs[2].n = 100;
    segs[3].s = 700; segs[3].n = 100;
    segs[4].s = 850; segs[4].n = 50;
    ref.n = 5;
    ref.union_seg[0].s = 100; ref.union_seg[0].e = 200;
    ref.union_seg[1].s = 300; ref.union_seg[1].e = 400;
    ref.union_seg[2].s = 500; ref.union_seg[2].e = 600;
    ref.union_seg[3].s = 700; ref.union_seg[3].e = 800;
    ref.union_seg[4].s = 850; ref.union_seg[4].e = 900;
    if (st_permutation(segs, n_seg, sizeof(st_int_seg_t),
            unit_test_int_seg_union_one, &ref) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    return 0;
}

static int unit_test_int_insert()
{
    int ref[] = {1, 2, 3, 4, 5, 1000};
    int A[128] = {0};
    int ncase = 0;

    int i, j, k, n, sz;

    n = sizeof(ref) / sizeof(ref[0]) - 1;
    fprintf(stderr, "  Testing st_int_insert...\n");
    fprintf(stderr, "    Case %d...", ncase++);
    for (i = 0; i < n; i++) {
        k = 0;
        for (j = 0; j < n; j++) {
            if (j == i) {
                continue;
            }
            A[k] = ref[j];
            k++;
        }

        sz = n - 1;
        if (st_int_insert(A, n, &sz, ref[i]) < 0) {
            fprintf(stderr, "Failed\n");
            return -1;
        }
        if (sz != n) {
            fprintf(stderr, "Failed\n");
            return -1;
        }
        for (j = 0; j < n; j++) {
            if (A[j] != ref[j]) {
                fprintf(stderr, "Failed\n");
                return -1;
            }
        }
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    sz = n;
    if (st_int_insert(A, n, &sz, ref[i]) >= 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    fprintf(stderr, "Success\n");

    for (i = 0; i < n; i++) {
        A[i] = ref[i];
    }
    fprintf(stderr, "    Case %d...", ncase++);
    sz = n;
    if (st_int_insert(A, n, &sz, ref[0]) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    if (sz != n) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    for (j = 0; j < n; j++) {
        if (A[j] != ref[j]) {
            fprintf(stderr, "Failed\n");
            return -1;
        }
    }
    fprintf(stderr, "Success\n");

    fprintf(stderr, "    Case %d...", ncase++);
    sz = n;
    if (st_int_insert(A, n + 1, &sz, ref[n]) < 0) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    if (sz != n + 1) {
        fprintf(stderr, "Failed\n");
        return -1;
    }
    for (j = 0; j < n + 1; j++) {
        if (A[j] != ref[j]) {
            fprintf(stderr, "Failed\n");
            return -1;
        }
    }
    fprintf(stderr, "Success\n");

    return 0;
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

    if (unit_test_int_seg_union() != 0) {
        ret = -1;
    }

    if (unit_test_int_insert() != 0) {
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
