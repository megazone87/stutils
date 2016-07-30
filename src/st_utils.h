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

#ifndef  _ST_UTILS_H_
#define  _ST_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

#include <stutils/st_macro.h>

const char* st_version();

long make_long(unsigned int a, unsigned int b);

void split_long(long l, unsigned int *a, unsigned int *b);

unsigned int highest_bit_mask(unsigned int num, int overflow);

uint32_t MurmurHash2 ( const void * key, int len, uint32_t seed );

int st_permutation(void *base, size_t n, size_t sz,
        int (*callback)(void *base, size_t n, void *args), void *args);

void st_qsort(void *const pbase, size_t total_elems, size_t size,
           int (*cmp) (const void *, const void *, void *), void *arg);

/*
 * Return value for a comparison function
 *
 * @param[in] elem1 first elem.
 * @param[in] elem2 second elem.
 * @param[in] args extra args.
 * @return a st_cmp_ret_t.
 */
typedef enum st_cmp_ret_t_ {
    ST_CMP_ERR     = -2, /**< error occurs when doing comparison. */
    ST_CMP_LESS    = -1, /**< first elem is less than second elem. */
    ST_CMP_EQUAL   = 0,  /**< first elem is equal to second elem. */
    ST_CMP_GREATER = 1,  /**< first elem is greater than second elem. */
} st_cmp_ret_t;

/*
 * comparison function
 *
 * @param[in] elem1 first elem.
 * @param[in] elem2 second elem.
 * @param[in] args extra args.
 * @return a st_cmp_ret_t.
 */
typedef st_cmp_ret_t (*st_cmp_func_t) (const void *elem1, const void *elem2,
        void *args);

/*
 * Insert and merge an elem into a sorted array.
 *
 * @param[in] base the array.
 * @param[in] cap capacity of array.
 * @param[in] sz size of one elem.
 * @param[in, out] num number of array.
 * @param[in] elem the elem to be inserted.
 * @param[in] cmp the comparison function.
 * @param[in] args args to the comparison function.
 * @return the index of inserted position, negtive value if any error.
 */
int st_insert(void *base, int cap, size_t sz, int *num, void *elem,
	    st_cmp_func_t cmp, void *arg);
#ifdef __cplusplus
}
#endif

#endif
