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
#include <stdio.h>
#include <assert.h>

#include "st_mem.h"

static int unit_test_st_aligned_malloc()
{
#define N 12
    char *ptr = NULL;
    size_t size = 123;
    size_t alignment;
    int i;
    int ncase;

    fprintf(stderr, " Testing st_aligned_malloc...\n");

    ncase = 1;
    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    for (i = 4; i <= N; i++) {
        alignment = 1 << i;
        ptr = st_aligned_malloc(size, alignment);
#ifdef _ST_TEST_DEBUG_
        printf("%zx, %p\n", alignment, ptr);
#endif
        if(((size_t)ptr & (alignment - 1)) != 0) {
            fprintf(stderr, "Failed\n");
            goto FAILED;
        }
        safe_st_aligned_free(ptr);
        fprintf(stderr, "Passed\n");
    }

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    for (i = 4; i <= N; i++) {
        ptr = st_aligned_malloc(size, (1 << i) - i);
        if(ptr != NULL) {
            fprintf(stderr, "Failed\n");
            goto FAILED;
        }
        safe_st_aligned_free(ptr);
        fprintf(stderr, "Passed\n");
    }

    return 0;

FAILED:
    safe_st_aligned_free(ptr);
    return -1;
}

static int unit_test_st_aligned_realloc()
{
#define N 12
    char *ptr = NULL;
    size_t size = 123;
    size_t alignment;
    int i;
    int ncase;

    fprintf(stderr, " Testing st_aligned_malloc...\n");

    ncase = 1;
    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    for (i = 4; i <= N; i++) {
        alignment = 1 << i;
        ptr = st_aligned_realloc(NULL, size, alignment);
#ifdef _ST_TEST_DEBUG_
        printf("%zx, %p\n", alignment, ptr);
#endif
        if(((size_t)ptr & (alignment - 1)) != 0) {
            fprintf(stderr, "Failed\n");
            goto FAILED;
        }
        safe_st_aligned_free(ptr);
        fprintf(stderr, "Passed\n");
    }

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    for (i = 4; i <= N; i++) {
        alignment = 1 << i;
        ptr = st_aligned_malloc(size, alignment);
        assert(ptr != NULL);
        ptr = st_aligned_realloc(ptr, size + 10, alignment);
#ifdef _ST_TEST_DEBUG_
        printf("%zx, %p\n", alignment, ptr);
#endif
        if(((size_t)ptr & (alignment - 1)) != 0) {
            fprintf(stderr, "Failed\n");
            goto FAILED;
        }
        safe_st_aligned_free(ptr);
        fprintf(stderr, "Passed\n");
    }

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    for (i = 4; i <= N; i++) {
        alignment = 1 << i;
        ptr = st_aligned_malloc(size, alignment);
        assert(ptr != NULL);
        alignment *= 2;
        ptr = st_aligned_realloc(ptr, size + 10, alignment);
#ifdef _ST_TEST_DEBUG_
        printf("%zx, %p\n", alignment, ptr);
#endif
        if(((size_t)ptr & (alignment - 1)) != 0) {
            fprintf(stderr, "Failed\n");
            goto FAILED;
        }
        safe_st_aligned_free(ptr);
        fprintf(stderr, "Passed\n");
    }

    return 0;

FAILED:
    safe_st_aligned_free(ptr);
    return -1;
}

static int run_all_tests()
{
    int ret = 0;

    if (unit_test_st_aligned_malloc() != 0) {
        ret = -1;
    }

    if (unit_test_st_aligned_realloc() != 0) {
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
