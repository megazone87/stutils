#ifndef  _ST_HEAP_H_
#define  _ST_HEAP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "st_macro.h"

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
    if(ptr != NULL) {\
        st_heap_destroy(ptr);\
        safe_free(ptr);\
        ptr = NULL;\
    }\
    } while(0)
void st_heap_destroy(st_heap_t* heap);

#ifdef __cplusplus
}
#endif

#endif

