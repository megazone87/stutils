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

#include "st_utils.h"
#include "st_log.h"
#include "st_queue.h"

st_queue_t* st_queue_create(st_queue_id_t capacity)
{
    st_queue_t* queue;
    void** data;

    queue = (st_queue_t*)malloc(sizeof(st_queue_t));
    if(NULL == queue) {
        ST_WARNING("alloc memory for queue failed");
        return NULL;
    }

    queue->capacity = capacity;
    queue->start_idx = 0;
    queue->end_idx = -1;
    data = (void**)malloc(sizeof(void*)*queue->capacity);
    if(NULL == data) {
        ST_WARNING("alloc memory for data failed");
        goto FAILED;
    }
    queue->data_arr = data;

    return queue;

FAILED:
    safe_st_queue_destroy(queue);
    return NULL;
}

int st_enqueue(st_queue_t* queue, void* obj)
{
    if(((queue->end_idx + 2) % queue->capacity) == queue->start_idx)
    {
        ST_WARNING("queue overflow");
        return ST_QUEUE_FULL;
    }
    queue->end_idx = ((queue->end_idx + 1) % queue->capacity);
    queue->data_arr[queue->end_idx] = obj;

    return ST_QUEUE_OK;
}

int st_dequeue(st_queue_t* queue, void** obj)
{
    if(((queue->end_idx + 1) % queue->capacity) == queue->start_idx)
    {
        ST_WARNING("queue empty.");
        return ST_QUEUE_EMPTY;
    }
    *obj = queue->data_arr[queue->start_idx];
    queue->start_idx = ((queue->start_idx + 1) % queue->capacity);

    return ST_QUEUE_OK;
}

int st_queue_empty(st_queue_t* queue)
{
    if(((queue->end_idx + 1) % queue->capacity) == queue->start_idx)
    {
        return ST_QUEUE_EMPTY;
    }
    return ST_QUEUE_OK;
}

st_queue_id_t st_queue_size(st_queue_t* queue)
{
    return ((queue->end_idx - queue->start_idx + 1) % queue->capacity);
}

int st_queue_clear(st_queue_t* queue)
{
    queue->start_idx = 0;
    queue->end_idx   = -1;
    return ST_QUEUE_OK;
}

void st_queue_destroy(st_queue_t* queue)
{
    if(queue == NULL) {
        return;
    }
    
    if(queue->data_arr != NULL) {
        safe_free(queue->data_arr);
    }
}
