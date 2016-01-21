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

#include <stutils/st_macro.h>
#include "st_log.h"
#include "st_utils.h"
#include "st_heap.h"

st_heap_t* st_heap_create(st_heap_id_t capacity, st_heap_cmp_func_t cmp,
        void *args, st_heap_id_t *heap_index, st_heap_id_t max_heap_index_num)
{
    st_heap_t* heap;

    ST_CHECK_PARAM(capacity <= 0, NULL);
    
    heap = (st_heap_t*)malloc(sizeof(st_heap_t));
    if(NULL == heap)
    {
        ST_WARNING("alloc memory for heap failed");
        return NULL;
    }

    heap->data_arr = (void**)malloc(sizeof(void*)*capacity);
    if(NULL == heap->data_arr)
    {
        ST_WARNING("alloc memory for data_arr failed");
        goto FAILED;
    }
    heap->capacity = capacity;
    heap->size = 0;

    heap->cmp = cmp;
    heap->args = args;
    heap->heap_index = heap_index;
    heap->max_heap_index_num = max_heap_index_num;

    return heap;

FAILED:
    safe_st_heap_destroy(heap);
    return NULL;
}

int st_heap_fixup(st_heap_t *heap, st_heap_id_t index)
{
    void **data_arr;
    st_heap_cmp_func_t cmp;
    void* args;
    st_heap_id_t   *heap_index;
    void* temp;
    st_heap_id_t parent;
    st_heap_id_t size;

    size = heap->size;
    if(size <= index)
    {
        ST_WARNING("wrong index[%u/%u] to st_heap_fixup", index, size);
        return ST_HEAP_ERR;
    }

    data_arr = heap->data_arr;
    cmp = heap->cmp;
    args = heap->args;
    heap_index = heap->heap_index;
    temp = data_arr[index];
    parent = (index - 1) / 2;
    //while(parent >= 0 && index != 0)
    while(index != 0)
    {
        if(cmp(data_arr[parent], data_arr[index], args) <= 0)
        {
            break;
        }

        data_arr[index] = data_arr[parent];
        data_arr[parent] = temp;
        if(heap_index)
        {
            heap_index[(st_heap_id_t)(long)data_arr[index]] = index;
            heap_index[(st_heap_id_t)(long)temp] = parent;
        }
        index = parent;
        if(index == 0)
        {
            break;
        }
        parent = (index - 1) / 2;
    }

    return ST_HEAP_OK;
}

int st_heap_fixdown(st_heap_t *heap, st_heap_id_t index)
{
    void **data_arr;
    st_heap_cmp_func_t cmp;
    void* args;
    st_heap_id_t   *heap_index;
    void* temp;
    st_heap_id_t child;
    st_heap_id_t size;

    size = heap->size;
    if(size <= index)
    {
        ST_WARNING("wrong index[%u/%u] to st_heap_fixdown", index, size);
        return ST_HEAP_ERR;
    }

    data_arr = heap->data_arr;
    cmp = heap->cmp;
    args = heap->args;
    heap_index = heap->heap_index;
    temp = data_arr[index];
    child = 2*index + 1;
    while(child < size)
    {
        if(child + 1 < size 
                && cmp(data_arr[child+1], data_arr[child], args) < 0)
        {
            ++child;
        }

        if(cmp(data_arr[child], data_arr[index], args) >= 0)
        {
            break;
        }

        data_arr[index] = data_arr[child];
        data_arr[child] = temp;
        if(heap_index)
        {
            heap_index[(st_heap_id_t)(long)data_arr[index]] = index;
            heap_index[(st_heap_id_t)(long)temp] = child;
        }
        index = child;
        child = 2 * index + 1;
    }

    return ST_HEAP_OK;
}

int st_heap_insert(st_heap_t* heap, void* obj)
{
    if(heap->size >= heap->capacity)
    {
        ST_WARNING("heap overflow");
        return ST_HEAP_FULL;
    }
    if(heap->heap_index && (st_heap_id_t)(long)obj >= heap->max_heap_index_num)
    {
        ST_WARNING("heap index overflow[%u/%u]", (st_heap_id_t)(long)obj,
                heap->max_heap_index_num);
        return ST_HEAP_FULL;
    }
    heap->data_arr[heap->size++] = obj;
    if(heap->heap_index)
    {
        heap->heap_index[(st_heap_id_t)(long)obj] = heap->size-1;
    }
    if(heap->size == 1)
    {
        return ST_HEAP_OK;
    }
    if(st_heap_fixup(heap, heap->size-1) != ST_HEAP_OK)
    {
        ST_WARNING("Failed to st_heap_fixup.");
        return ST_HEAP_ERR;
    }

    return ST_HEAP_OK;
}

int st_heap_extract(st_heap_t* heap, void** obj)
{
    if(heap->size <= 0)
    {
        ST_WARNING("heap empty");
        return ST_HEAP_EMPTY;
    }
    *obj = heap->data_arr[0];
    heap->size--;

    if(heap->heap_index)
    {
        heap->heap_index[(st_heap_id_t)(long)*obj] = -1;
    }

    if(heap->size == 0)
    {
        return ST_HEAP_OK;
    }

    heap->data_arr[0] = heap->data_arr[heap->size];
    if(heap->heap_index)
    {
        heap->heap_index[(st_heap_id_t)(long)heap->data_arr[0]] = 0;
    }
    if(st_heap_fixdown(heap, 0) != ST_HEAP_OK)
    {
        ST_WARNING("Failed to st_heap_fixdown.");
        return ST_HEAP_ERR;
    }

    return ST_HEAP_OK;
}

int st_heap_empty(st_heap_t* heap)
{
    return heap->size == 0;
}

st_heap_id_t st_heap_size(st_heap_t* heap)
{
    return heap->size;
}

int st_heap_clear(st_heap_t* heap)
{
    heap->size = 0;
    return ST_HEAP_OK;
}

void st_heap_destroy(st_heap_t* heap)
{
    if(heap == NULL) {
        return;
    }
    
    if(heap->data_arr != NULL) {
        safe_free(heap->data_arr);
    }
}
