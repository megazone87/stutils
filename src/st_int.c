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
#include "st_int.h"

int st_parse_int_array(const char *str, int **arr, int *n_arr)
{
    const char *q;
    bool neg;
    int n;

    q = str;
    n = INT_MAX;
    neg = false;

    while (*q != 0) {
        if (*q == ',') {
            if (n == INT_MAX) {
                ST_WARNING("Error array: No number found before ','");
                return -1;
            }
            *arr = (int *)realloc(*arr, sizeof(int)*(*n_arr + 1));
            if (*arr == NULL) {
                ST_WARNING("Failed to realloc array[%d].", *n_arr);
                return -1;
            }
            if (neg) {
                n = -n;
            }
            (*arr)[*n_arr] = n;
            ++(*n_arr);
            n = INT_MAX;
            neg = false;
        } else {
            if (*q == '-') {
                if (n != INT_MAX) {
                    ST_WARNING("Error array: found not proper '-'");
                    return -1;
                }
                neg = true;
            } else if (*q < '0' || *q > '9') {
                ST_WARNING("Error array: found non-digit[%c]", *q);
                return -1;
            } else {
                if (n == INT_MAX) {
                    n = (*q - '0');
                } else {
                    n = n * 10 + (*q - '0');
                }
            }
        }

        ++q;
    }
    if (n == INT_MAX) {
        ST_WARNING("Error array: extra ',' found in the end");
        return -1;
    }
    *arr = (int *)realloc(*arr, sizeof(int)*(*n_arr + 1));
    if (*arr == NULL) {
        ST_WARNING("Failed to realloc array[%d].", *n_arr);
        return -1;
    }
    if (neg) {
        n = -n;
    }
    (*arr)[*n_arr] = n;
    ++(*n_arr);

    return 0;
}

int st_parse_wt_int_array(const char *str, st_wt_int_t **arr, int *n_arr,
        float def_wt)
{
    const char *q;
    bool neg;
    int n;

    bool wt_neg;
    bool in_wt;
    bool in_frac;
    int i;
    float f;
    int power;


    q = str;
    n = INT_MAX;
    neg = false;

    i = INT_MAX;
    f = FLT_MAX;
    wt_neg = false;
    in_wt = false;
    in_frac = false;
    power = 1;

    while (*q != 0) {
        if (*q == ',') {
            if (n == INT_MAX) {
                ST_WARNING("Error array: No number found before ','");
                return -1;
            }
            *arr = (st_wt_int_t *)realloc(*arr,
                    sizeof(st_wt_int_t)*(*n_arr + 1));
            if (*arr == NULL) {
                ST_WARNING("Failed to realloc array[%d].", *n_arr);
                return -1;
            }
            if (neg) {
                n = -n;
            }
            (*arr)[*n_arr].i = n;
            if (i == INT_MAX) {
                (*arr)[*n_arr].w = def_wt;
            } else {
                if (f == FLT_MAX) {
                    f = 0;
                }
                (*arr)[*n_arr].w = i + f;
                if (wt_neg) {
                    (*arr)[*n_arr].w = -(*arr)[*n_arr].w;
                }
            }

            ++(*n_arr);
            n = INT_MAX;
            neg = false;

            i = INT_MAX;
            f = FLT_MAX;
            wt_neg = false;
            in_wt = false;
            in_frac = false;
            power = 1;
        } else {
            if (in_wt) {
                if (*q == '-') {
                    if (i != INT_MAX) {
                        ST_WARNING("Error array: found not proper '-'");
                        return -1;
                    }
                    wt_neg = true;
                } else if (*q == '.') {
                    if (f != FLT_MAX) {
                        ST_WARNING("Error array: found not proper '-'");
                        return -1;
                    }
                    in_frac = true;
                    if (i == FLT_MAX) {
                        i = 0;
                    }
                } else if (*q < '0' || *q > '9') {
                    ST_WARNING("Error array: found non-digit[%c]", *q);
                    return -1;
                } else {
                    if (f == FLT_MAX) {
                        if (in_frac) {
                            power = 1;
                            f = pow(10, -power) * (*q - '0');
                        } else {
                            i = (*q - '0');
                        }
                    } else {
                        if (in_frac) {
                            ++power;
                            f = f + pow(10, -power) * (*q - '0');
                        } else {
                            i = i * 10 + (*q - '0');
                        }
                    }
                }
            } else {
                if (*q == ':') {
                    in_wt = true;
                } else {
                    if (*q == '-') {
                        if (n != INT_MAX) {
                            ST_WARNING("Error array: found not proper '-'");
                            return -1;
                        }
                        neg = true;
                    } else if (*q < '0' || *q > '9') {
                        ST_WARNING("Error array: found non-digit[%c]", *q);
                        return -1;
                    } else {
                        if (n == INT_MAX) {
                            n = (*q - '0');
                        } else {
                            n = n * 10 + (*q - '0');
                        }
                    }
                }
            }
        }

        ++q;
    }
    if (n == INT_MAX) {
        ST_WARNING("Error array: extra ',' found in the end");
        return -1;
    }
    *arr = (st_wt_int_t *)realloc(*arr,
            sizeof(st_wt_int_t)*(*n_arr + 1));
    if (*arr == NULL) {
        ST_WARNING("Failed to realloc array[%d].", *n_arr);
        return -1;
    }
    if (neg) {
        n = -n;
    }
    (*arr)[*n_arr].i = n;
    if (i == INT_MAX) {
        (*arr)[*n_arr].w = def_wt;
    } else {
        if (f == FLT_MAX) {
            f = 0;
        }
        (*arr)[*n_arr].w = i + f;
        if (wt_neg) {
            (*arr)[*n_arr].w = -(*arr)[*n_arr].w;
        }
    }

    ++(*n_arr);

    return 0;
}

static int int_comp(const void *elem1, const void *elem2)
{
    int f = *((int*)elem1);
    int s = *((int*)elem2);
    if (f > s) return  1;
    if (f < s) return -1;
    return 0;
}

void st_int_sort(int *A, size_t n)
{
    qsort(A, n, sizeof(int), int_comp);
}

static int wt_int_comp(const void *elem1, const void *elem2)
{
    st_wt_int_t* f = (st_wt_int_t*)elem1;
    st_wt_int_t* s = (st_wt_int_t*)elem2;
    if (f->i > s->i) return  1;
    if (f->i < s->i) return -1;
    return 0;
}

void st_wt_int_sort(st_wt_int_t *A, size_t n)
{
    qsort(A, n, sizeof(st_wt_int_t), wt_int_comp);
}

static int st_int_seg_union_insert(st_int_seg_t *segs, int cap_seg,
        int *n_seg, int s, int e)
{
    int i;
    int j;
    int n;

    ST_CHECK_PARAM_EX(segs == NULL || n_seg == NULL
            || s < 0 || e < 0 || s >= e, -1, "%d, %d", s, e);

    n = *n_seg;

    if (n > 0) {
        if (e < segs[0].s) {
            if (*n_seg >= cap_seg) {
                ST_WARNING("Overflow");
                return -1;
            }
            memmove(segs + 1, segs, sizeof(st_int_seg_t)*n);
            segs[0].s = s;
            segs[0].e = e;
            (*n_seg)++;
            return 0;
        } else if (e == segs[0].s) {
            segs[0].s = s;
            return 0;
        }
    }

    // skip non-intersected sets
    for (i = 0; i < n; i++) {
        if (s <= segs[i].e) {
            break;
        }
    }

    if (i == n) {
        if (*n_seg >= cap_seg) {
            ST_WARNING("Overflow");
            return -1;
        }
        segs[n].s = s;
        segs[n].e = e;

        (*n_seg)++;
        return 0;
    }

    if (e < segs[i].s) { // insert new set before ith
        if (*n_seg >= cap_seg) {
            ST_WARNING("Overflow");
            return -1;
        }
        memmove(segs + i + 1, segs + i, sizeof(st_int_seg_t) * (n - i));
        segs[i].s = s;
        segs[i].e = e;
        (*n_seg)++;
    } else {// forward extend the ith set
        for (j = i + 1; j < n; j++) {
            if (e <= segs[j].s) {
                break;
            }
        }

        segs[i].s = min(s, segs[i].s);
        if (j == n || e < segs[j].s) {
            segs[i].e = max(e, segs[j - 1].e);
            if (j < n) {
                memmove(segs + i + 1, segs + j,
                        sizeof(st_int_seg_t) * (n - j));
            }
            *n_seg -= (j - i - 1);
        } else {
            segs[i].e = max(e, segs[j].e);

            if (j + 1 < n) {
                memmove(segs + i + 1, segs + j + 1,
                        sizeof(st_int_seg_t) * (n - (j + 1)));
            }
            *n_seg -= (j - i);
        }
    }

    return 0;
}

int st_int_seg_union(st_int_seg_t *union_segs, int cap_union,
        int *n_union, st_int_seg_t *segs, int n_seg, int size)
{
    int s;
    int e;

    int i;

    ST_CHECK_PARAM(union_segs == NULL || n_union == NULL
            || *n_union < 0  || segs == NULL, -1);

    for (i = 0; i < n_seg; i++) {
        if (segs[i].s + segs[i].n > size) {
            s = segs[i].s;
            e = size;
            if (st_int_seg_union_insert(union_segs, cap_union,
                        n_union, s, e) < 0) {
                ST_WARNING("Failed to st_int_seg_union_insert.");
                return -1;
            }

            s = 0;
            e = segs[i].n - (size - segs[i].s);
            if (st_int_seg_union_insert(union_segs, cap_union,
                        n_union, s, e) < 0) {
                ST_WARNING("Failed to st_int_seg_union_insert.");
                return -1;
            }
        } else {
            s = segs[i].s;
            e = segs[i].s + segs[i].n;
            if (st_int_seg_union_insert(union_segs, cap_union,
                        n_union, s, e) < 0) {
                ST_WARNING("Failed to st_int_seg_union_insert.");
                return -1;
            }
        }
    }

    return 0;
}

int st_int_insert(int *A, int cap, int *sz, int n)
{
    int i;

    ST_CHECK_PARAM(A == NULL || sz == NULL || *sz < 0, -1);

    for (i = 0; i < *sz; i++) {
        if (A[i] == n) {
            return 0;
        } else if (A[i] > n) {
            break;
        }
    }

    if (*sz + 1 > cap) {
        ST_WARNING("overflow");
        return -1;
    }

    if (i >= *sz) {
        A[*sz] = n;
    } else {
        memmove(A + i + 1, A + i, (*sz - i) * sizeof(int));
        A[i] = n;
    }
    *sz += 1;

    return 0;
}
