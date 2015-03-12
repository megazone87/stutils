#ifndef  _ST_STACK_H_
#define  _ST_STACK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "st_macro.h"

#define ST_STACK_OK    0
#define ST_STACK_ERR   -1
#define ST_STACK_FULL  1
#define ST_STACK_EMPTY 2

typedef unsigned int st_stack_id_t;

typedef struct _st_stack_t_
{
    st_stack_id_t capacity;
    st_stack_id_t top;
    void** data_arr;
} st_stack_t;

st_stack_t* st_stack_create(st_stack_id_t capacity);

#define safe_st_stack_destroy(ptr) do {\
    if(ptr != NULL) {\
        st_stack_destroy(ptr);\
        safe_free(ptr);\
        ptr = NULL;\
    }\
    } while(0)
void st_stack_destroy(st_stack_t *stack);

int st_stack_push(st_stack_t* st_stack, void* obj);

int st_stack_pop(st_stack_t* st_stack, void** obj);

int st_stack_clear(st_stack_t* st_stack);

bool st_stack_empty(st_stack_t* st_stack);

int st_stack_top(st_stack_t* st_stack, void** obj);

int st_stack_topn(st_stack_t* st_stack, st_stack_id_t n, void** obj);

#ifdef __cplusplus
}
#endif

#endif

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
