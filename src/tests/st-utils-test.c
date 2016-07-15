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

static int run_all_tests()
{
    int ret = 0;

    if (unit_test_permutation() != 0) {
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
