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

#ifndef  _ST_HEAP_H_
#define  _ST_HEAP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stutils/st_macro.h>

#define ST_HEAP_OK    0
#define ST_HEAP_ERR   -1
#define ST_HEAP_FULL  1 
#define ST_HEAP_EMPTY 2

typedef unsigned int st_heap_id_t;

struct _st_heap_t_;

typedef int (*st_heap_cmp_func_t)(void *i, void *j, void *args);

typedef struct _st_heap_t_
{
    void** data_arr;
    st_heap_id_t size;
    st_heap_id_t capacity;

    st_heap_cmp_func_t  cmp;
    void                 *args;
    st_heap_id_t             *heap_index;
    st_heap_id_t             max_heap_index_num;
} st_heap_t;

st_heap_t* st_heap_create(st_heap_id_t capacity, st_heap_cmp_func_t cmp,
        void *args, st_heap_id_t *heap_index, st_heap_id_t max_heap_index_num);

int st_heap_insert(st_heap_t* heap, void* obj);

int st_heap_extract(st_heap_t* heap, void** obj);

int st_heap_empty(st_heap_t* heap);

st_heap_id_t st_heap_size(st_heap_t* heap);

int st_heap_clear(st_heap_t* heap);

int st_heap_fixdown(st_heap_t *heap, st_heap_id_t index);

int st_heap_fixup(st_heap_t *heap, st_heap_id_t index);

#define safe_st_heap_destroy(ptr) do {\
    if((ptr) != NULL) {\
        st_heap_destroy(ptr);\
        safe_free(ptr);\
        (ptr) = NULL;\
    }\
    } while(0)
void st_heap_destroy(st_heap_t* heap);

#ifdef __cplusplus
}
#endif

#endif
