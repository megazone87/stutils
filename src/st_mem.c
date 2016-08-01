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

#include <stutils/st_macro.h>
#include "st_log.h"
#include "st_mem.h"

void* st_aligned_malloc(size_t size, size_t alignment)
{
    void *p1; // original block
    void *p2; // aligned block
    size_t *p3;
    size_t padding;

    if (!is_power_of_two(alignment)) {
        ST_WARNING("alignment[%zu] is not power of 2.", alignment);
        return NULL;
    }

    padding = alignment - 1 + 3 * sizeof(size_t);
    p1 = (void *)malloc(size + padding);
    if (p1 == NULL) {
        ST_WARNING("Failed to malloc[%zu].", size + padding);
        return NULL;
    }
    p2 = (void *)(((size_t)p1 + padding) & ~(alignment - 1)); // insert padding
    p3 = (size_t *)p2;
    p3[-1] = (char *)p2 - (char *)p1; // store offset
    p3[-2] = alignment; // store alignment
    p3[-3] = size; // store size

    return p2;
}

void* st_aligned_realloc(void *ptr, size_t size, size_t alignment)
{
    void *p1, *q1; // original block
    void *q2; // aligned block
    size_t *p3;
    size_t ori_alignment, ori_offset, ori_size;
    size_t padding;

    if (!is_power_of_two(alignment)) {
        ST_WARNING("alignment[%zu] is not power of 2.", alignment);
        return NULL;
    }

    if (ptr == NULL) {
        q2 = st_aligned_malloc(size, alignment);
        if (q2 == NULL) {
            ST_WARNING("Failed to st_aligned_malloc.");
            return NULL;
        }

        return q2;
    }

    p3 = (size_t *)ptr;
    ori_offset = p3[-1];
    ori_alignment = p3[-2];
    ori_size = p3[-3];

    p1 = (char *)ptr - ori_offset;

    padding = alignment - 1 + 3 * sizeof(size_t);
    q1 = (void *)realloc(p1, size + padding);
    if (q1 == NULL) {
        ST_WARNING("Failed to realloc[%zu].", size + padding);
        return NULL;
    }

    if (alignment != ori_alignment) {
        goto REALIGN;
    }

    if (p1 == q1) { /* block is expanded in-place. */
        p3 = (size_t *)ptr;
        p3[-3] = size;

        return ptr;
    }

    q2 = q1 + ori_offset;
    if (((size_t)q2 & (alignment - 1)) == 0) {
        /* realloc happens to give us a  correctly aligned block. */
        p3 = (size_t *)q2;
        p3[-3] = size;

        return q2;
    }

REALIGN:
    /* realign the block. */
    q2 = (void *)(((size_t)q1 + padding) & ~(alignment - 1)); // insert padding
    memmove(q2, q1 + ori_offset, ori_size);
    p3 = (size_t *)q2;
    p3[-1] = (char *)q2 - (char *)q1; // store offset
    p3[-2] = alignment; // store alignment
    p3[-3] = size; // store size

    return q2;
}

void st_aligned_free(void *p)
{
    void *p1;
    size_t *p3;

    p3 = (size_t *)p;
    p1 = (char *)p - p3[-1];

    free(p1);
}
