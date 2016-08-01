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

#ifndef  _ST_MEM_H_
#define  _ST_MEM_H_

#ifdef __cplusplus
extern "C" {
#endif

#define is_power_of_two(x) (((x) != 0) && !((x) & ((x) - 1)))

/*
 * alloc aligned memory block.
 *
 * @param[in] size size of bytes for alloc.
 * @param[in] alignment size of alignment. Must be power of 2.
 * @return pointer to the alloced memory, NULL if any error.
 */
void* st_aligned_malloc(size_t size, size_t alignment);

/*
 * realloc aligned memory block.
 *
 * @param[in] ptr original block. This pointer must be the one returned
 *                from st_aligned_malloc or st_aligned_realloc.
 *                If ptr == NULL, it will return a new aligned block.
 * @param[in] size new size of block.
 * @param[in] alignment size of alignment. Must be power of 2. If it is not
 *                      equal the alignment of ptr, it will be realigned.
 * @return pointer to the realloced memory, NULL if any error.
 */
void* st_aligned_realloc(void *ptr, size_t size, size_t alignment);

#define safe_st_aligned_free(ptr) do {\
    if((ptr) != NULL) {\
        st_aligned_free(ptr);\
        (ptr) = NULL;\
    }\
    } while(0)
/*
 * free a aligned memory block.
 *
 * @param[in] ptr memory block. This pointer must be the one returned
 *                from st_aligned_malloc or st_aligned_realloc.
 */
void st_aligned_free(void *p);

#ifdef __cplusplus
}
#endif

#endif
