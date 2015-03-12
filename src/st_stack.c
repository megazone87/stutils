#include <string.h>
#include "st_stack.h"

void st_stack_destroy(st_stack_t *stack)
{
    if (stack == NULL) {
        return;
    }

    if(stack->data_arr) {
        safe_free(stack->data_arr);
    }
}

st_stack_t* st_stack_create(st_stack_id_t capacity)
{
    st_stack_t* st_stack = NULL;

    ST_CHECK_PARAM(capacity <= 0, NULL);

    st_stack = (st_stack_t*)malloc(sizeof(st_stack_t));
    if(NULL == st_stack)
    {
        ST_WARNING("alloc memory for st_stack failed");
        return NULL;
    }
    memset(st_stack, 0, sizeof(st_stack_t));
    st_stack->capacity = capacity;
    st_stack->top = 0;

    st_stack->data_arr = (void **)malloc(sizeof(void *)*capacity);
    if(NULL == st_stack->data_arr)
    {
        ST_WARNING("alloc memory for data_arr failed");
        goto ERR;
    }
    memset(st_stack->data_arr, 0, sizeof(void*)*capacity);

    return st_stack;

ERR:
    safe_st_stack_destroy(st_stack);
    return NULL;
}

int st_stack_push(st_stack_t* st_stack, void* obj)
{
    if(st_stack->top == st_stack->capacity)
    {
        ST_WARNING("st_stack overflow");
        return ST_STACK_FULL;
    }

    st_stack->data_arr[st_stack->top++] = obj;

    return ST_STACK_OK;
}

int st_stack_pop(st_stack_t* st_stack, void** obj)
{
    if(st_stack->top == 0)
    {
        ST_WARNING("st_stack empty");
        return ST_STACK_EMPTY;
    }

    *obj = st_stack->data_arr[--st_stack->top];
    
    return ST_STACK_OK;
}

int st_stack_clear(st_stack_t* st_stack)
{
    st_stack->top = 0;
    return ST_STACK_OK;
}

bool st_stack_empty(st_stack_t* st_stack)
{
    return (st_stack->top == 0);
}

int st_stack_top(st_stack_t* st_stack, void** obj)
{
    if(st_stack->top == 0)
    {
        return ST_STACK_EMPTY;
    }

    *obj = st_stack->data_arr[st_stack->top-1];
    
    return ST_STACK_OK;
}

int st_stack_topn(st_stack_t* st_stack, st_stack_id_t n, void** obj)
{
    if(st_stack->top <= n - 1)
    {
        ST_WARNING("st_stack empty");
        return ST_STACK_EMPTY;
    }

    *obj = st_stack->data_arr[st_stack->top-n];
    
    return ST_STACK_OK;
}

