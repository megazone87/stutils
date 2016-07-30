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

#ifndef  _ST_INT_H_
#define  _ST_INT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

#include <stutils/st_macro.h>

/**
 * Parse int array from a comma seperated string.
 *
 * @param[in] str input string.
 * @param[out] arr output int array, may be malloced/realloced.
 * @param[out] n_arr number of ints.
 * @return non-zero value if any error.
 */
int st_parse_int_array(const char *str, int **arr, int *n_arr);

/**
 * Weighted integer.
 */
typedef struct _st_weighted_int_t_ {
    int i;   /**< integer */
    float w; /**< weight. */
} st_wt_int_t;

/**
 * Parse weighted int array from a comma seperated string.
 *
 * @param[in] str input string.
 * @param[out] arr output int array, may be malloced/realloced.
 * @param[out] n_arr number of ints.
 * @param[in] def_wt default value for weight.
 * @return non-zero value if any error.
 */
int st_parse_wt_int_array(const char *str, st_wt_int_t **arr, int *n_arr,
        float def_wt);

/**
 * Sort int array.
 *
 * @param[in] A array.
 * @param[in] n length of array.
 */
void st_int_sort(int *A, size_t n);

/**
 * Sort weighted int array.
 *
 * @param[in] A array.
 * @param[in] n length of array.
 */
void st_wt_int_sort(st_wt_int_t *A, size_t n);

/**
 * integer segment.
 */
typedef struct _st_int_segment_t_ {
    int s;   /**< start of segment */
    union {
        int e; /**< end of segment. */
        int n; /**< lenght of segment. */
    };
} st_int_seg_t;

/*
 * Find union of n_seg sets of segs
 *
 * @param[in] union_segs the union set(set seg.s and seg.e).
 * @param[in,out] cap_union capacity of segs in union_segs.
 * @param[in,out] n_union number of segs in union_segs.
 * @param[in] segs sets of segs(using seg.s and seg.n).
 * @param[in] n_seg number of segs.
 * @param[in] max_e max possible value of seg.e.
 * @return non-zero value if any error.
 */
int st_int_seg_union(st_int_seg_t *union_segs, int cap_union, int *n_union,
        st_int_seg_t *segs, int n_seg, int max_e);

/*
 * Insert and merge a integer into a sorted array.
 *
 * @param[in] A the int array.
 * @param[in] cap capacity of A.
 * @param[in, out] sz size of A.
 * @param[in] n the integet.
 * @return non-zero value if any error.
 */
int st_int_insert(int *A, int cap, int *sz, int n);

#ifdef __cplusplus
}
#endif

#endif
