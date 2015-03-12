#ifndef  _ST_QUEUE_H_
#define  _ST_QUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "st_macro.h"

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
    if(ptr != NULL) {\
        st_queue_destroy(ptr);\
        safe_free(ptr);\
        ptr = NULL;\
    }\
    } while(0)
void st_queue_destroy(st_queue_t* queue);

#ifdef __cplusplus
}
#endif

#endif

