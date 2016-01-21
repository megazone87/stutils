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

#ifndef  _ST_QUEUE_H_
#define  _ST_QUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stutils/st_macro.h>

#define ST_QUEUE_OK    0
#define ST_QUEUE_ERR   -1
#define ST_QUEUE_FULL  1 
#define ST_QUEUE_EMPTY 2

typedef unsigned int st_queue_id_t;

typedef struct _st_st_queue_t_
{
    st_queue_id_t capacity;
    st_queue_id_t start_idx;
    st_queue_id_t end_idx;
    void** data_arr;
}st_queue_t;

st_queue_t* st_queue_create(st_queue_id_t capacity);

int st_enqueue(st_queue_t* queue, void* obj);

int st_dequeue(st_queue_t* queue, void** obj);

int st_queue_empty(st_queue_t* queue);

st_queue_id_t st_queue_size(st_queue_t* queue);

int st_queue_clear(st_queue_t* queue);

#define safe_st_queue_destroy(ptr) do {\
    if((ptr) != NULL) {\
        st_queue_destroy(ptr);\
        safe_free(ptr);\
        (ptr) = NULL;\
    }\
    } while(0)
void st_queue_destroy(st_queue_t* queue);

#ifdef __cplusplus
}
#endif

#endif
