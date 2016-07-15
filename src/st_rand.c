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
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <sys/stat.h>

#include <stutils/st_macro.h>
#include "st_log.h"
#include "st_rand.h"

static unsigned st_rand_state = -1726662223;

int st_rand()
{
    return st_rand_r(&st_rand_state);
}

/* the magic numbers is from stdlibc */
int st_rand_r(unsigned *seed)
{
    *seed = (((*seed) * 1103515245) + 12345) & 0x7fffffff;
    return (int)*seed;
}

void st_srand(unsigned int seed)
{
    st_rand_state = seed;
}

double st_random(double min, double max)
{
    return st_rand() / (double) ST_RAND_MAX *(max - min) + min;
}

void st_shuffle(int *a, size_t n)
{
    size_t i;
    size_t j;
    int t;

    if (n <= 0) {
        return;
    }

    i = n - 1;
    while (i > 1) {
        j = (size_t)((double)i * ( st_rand() / (ST_RAND_MAX + 1.0) ));

        t = a[j];
        a[j] = a[i];
        a[i] = t;
        i--;
    }
}

void st_shuffle_r(int *a, size_t n, unsigned *seed)
{
    size_t i;
    size_t j;
    int t;

    if (n <= 0) {
        return;
    }

    i = n - 1;
    while (i > 1) {
        j = (size_t)((double)i * ( st_rand_r(seed) / (ST_RAND_MAX + 1.0) ));

        t = a[j];
        a[j] = a[i];
        a[i] = t;
        i--;
    }
}

double st_gaussrand()
{
    static double V1, V2, S;
    static int phase = 0;
    double X;

    if(phase == 0) {
        do {
            double U1 = (double)st_rand() / ST_RAND_MAX;
            double U2 = (double)st_rand() / ST_RAND_MAX;

            V1 = 2 * U1 - 1;
            V2 = 2 * U2 - 1;
            S = V1 * V1 + V2 * V2;
        } while(S >= 1 || S == 0);

        X = V1 * sqrt(-2 * log(S) / S);
    } else
        X = V2 * sqrt(-2 * log(S) / S);

    phase = 1 - phase;

    return X;
}

double st_normrand(double mean, double stdev)
{
    return ((stdev * st_gaussrand()) + mean);
}

void st_gauss_r_init(st_gauss_r_t *gauss, double mean,
        double stdev, unsigned seed)
{
    memset(gauss, 0, sizeof(st_gauss_r_t));
    gauss->mean = mean;
    gauss->stdev = stdev;
    gauss->seed = seed;
}

double st_gaussrand_r(st_gauss_r_t *gauss)
{
    double U1;
    double U2;
    double X;

    if(gauss->phase == 0) {
        do {
            U1 = (double)st_rand_r(&gauss->seed) / ST_RAND_MAX;
            U2 = (double)st_rand_r(&gauss->seed) / ST_RAND_MAX;

            gauss->V1 = 2 * U1 - 1;
            gauss->V2 = 2 * U2 - 1;
            gauss->S = gauss->V1 * gauss->V1 + gauss->V2 * gauss->V2;
        } while(gauss->S >= 1 || gauss->S == 0);

        X = gauss->V1 * sqrt(-2 * log(gauss->S) / gauss->S);
    } else
        X = gauss->V2 * sqrt(-2 * log(gauss->S) / gauss->S);

    gauss->phase = 1 - gauss->phase;

    return ((gauss->stdev * X) + gauss->mean);
}
