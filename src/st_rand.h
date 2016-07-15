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

#ifndef  _ST_RAND_H_
#define  _ST_RAND_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

#include <stutils/st_macro.h>

/* The largest number rand will return (same as INT_MAX).  */
#define ST_RAND_MAX        2147483647
int st_rand();
double st_random(double min, double max);
int st_rand_r(unsigned int *seed);
void st_srand(unsigned int seed);

void st_shuffle(int *a, size_t n);
void st_shuffle_r(int *a, size_t n, unsigned *rand);

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

#ifdef __cplusplus
}
#endif

#endif
