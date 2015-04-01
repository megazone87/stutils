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

#include <string.h>
#include <assert.h>
#include "st_macro.h"
#include "st_utils.h"
#include "st_log.h"
#include "st_dict.h"

void st_dict_destroy(st_dict_t *wd)
{
    if(wd == NULL) {
        return;
    }

    if(wd->first_level_node) {
        safe_free(wd->first_level_node);
    }

    if(wd->node_pool) {
        safe_free(wd->node_pool);
    }
    
    if(wd->clear_nodes) {
        safe_free(wd->clear_nodes);
    }
}

st_dict_id_t st_dict_hash_simple(st_dict_t *wd, st_dict_node_t *pnode)
{
    return (pnode->sign1 + pnode->sign2)&(wd->addr_mask);
}

st_dict_id_t st_dict_hash_sign1l16(st_dict_t *wd, st_dict_node_t *pnode)
{
    return ((pnode->sign1 << 16) ^ pnode->sign2)&(wd->addr_mask);
}

st_dict_id_t st_dict_hash_sign1(st_dict_t *wd, st_dict_node_t *pnode)
{
    return (pnode->sign1 & wd->addr_mask);
}

bool st_dict_node_equal(st_dict_node_t *node1, st_dict_node_t *node2,
    void *arg )
{
    return ((node1->sign1 == node2->sign1) && (node1->sign2 == node2->sign2));
}

static st_dict_t* st_dict_alloc()
{
    st_dict_t *wd;

    wd = (st_dict_t *)malloc(sizeof(st_dict_t));
    if(wd == NULL)
    {
        ST_WARNING("Failed to alloc mem for st_dict.");
        return NULL;
    }
    memset(wd, 0, sizeof(st_dict_t));

    return wd;
}

st_dict_t* st_dict_create(st_dict_id_t hash_num,
    st_dict_id_t realloc_node_num, st_dict_hash_fun_t hash_func,
    st_dict_node_eq_fun_t node_eq_func, bool need_clear)
{
    st_dict_t      *wd;
    st_dict_id_t   i;

    ST_CHECK_PARAM(hash_num == ST_DICT_BAD_NODE 
            || realloc_node_num == ST_DICT_BAD_NODE, NULL);

    wd = (st_dict_t *)malloc(sizeof(st_dict_t));
    if(wd == NULL)
    {
        ST_WARNING("Failed to alloc mem for st_dict.");
        return NULL;
    }
    bzero(wd, sizeof(st_dict_t));
    wd->realloc_node_num = realloc_node_num;
    if(hash_func)
    {
        wd->hash_func = hash_func;
    }
    else
    {
        wd->hash_func = st_dict_hash_simple;
    }
    if(node_eq_func)
    {
        wd->node_eq_func = node_eq_func;
    }
    else
    {
        wd->node_eq_func = st_dict_node_equal;
    }

    wd->addr_mask = highest_bit_mask(hash_num, false);
    wd->hash_num = wd->addr_mask + 1;
    //ST_DEBUG("num=%d(0x%x), mask=0x%x, num=%d(0x%x)", hash_num, hash_num,
        //wd->addr_mask, wd->hash_num, wd->hash_num);
    wd->first_level_node = (st_dict_node_t *)
        malloc(sizeof(st_dict_node_t) * wd->hash_num);
    if(wd->first_level_node == NULL)
    {
        ST_WARNING("Failed to alloc mem for first_level_node.");
        goto FAILED;
    }

    wd->node_pool = (st_dict_node_t *)
        malloc(sizeof(st_dict_node_t)*wd->hash_num);
    if(wd->node_pool == NULL)
    {
        ST_WARNING("Failed to alloc mem for node_pool.");
        goto FAILED;
    }
    
    if(need_clear)
    {
        wd->clear_nodes = (st_dict_id_t *)
            malloc(sizeof(st_dict_id_t)*wd->hash_num);
        if(wd->clear_nodes == NULL)
        {
            ST_WARNING("Failed to alloc mem for clear_nodes.");
            goto FAILED;
        }
        wd->clear_node_num = 0;
    }

    for(i = 0; i < wd->hash_num; i++)
    {
        wd->first_level_node[i].sign1 = 0;
        wd->first_level_node[i].sign2 = 0;
        wd->first_level_node[i].uint1 = 0;
        wd->first_level_node[i].next = ST_DICT_BAD_NODE;
        
        wd->node_pool[i].sign1 = 0;
        wd->node_pool[i].sign2 = 0;
        wd->node_pool[i].uint1 = 0;
        wd->node_pool[i].next = ST_DICT_BAD_NODE;
    }
    wd->node_num = 0;
    wd->max_pool_num = wd->hash_num;
    wd->cur_index = 0;

    return wd;

FAILED:
    safe_st_dict_destroy(wd);
    return NULL;
}

static st_dict_id_t st_dict_add_in(st_dict_t *wd, st_dict_node_t *pnode)
{
    st_dict_node_t *node;

    if(wd->cur_index >= wd->max_pool_num)
    {
        wd->node_pool = (st_dict_node_t *)realloc(wd->node_pool,
            (wd->max_pool_num + wd->realloc_node_num)*sizeof(st_dict_node_t));
        if(wd->node_pool == NULL)
        {
            ST_WARNING("Realloc node_pool failed.");
            return ST_DICT_BAD_NODE;
        }
        bzero(wd->node_pool + wd->max_pool_num, wd->realloc_node_num*sizeof(st_dict_node_t));

        wd->max_pool_num += wd->realloc_node_num;
    }
    node = &wd->node_pool[wd->cur_index];
    node->sign1 = pnode->sign1;
    node->sign2 = pnode->sign2;
    node->uint1 = pnode->uint1;
    node->next = ST_DICT_BAD_NODE;

    return wd->cur_index++;
}

int st_dict_add(st_dict_t *wd, st_dict_node_t *pnode, void *node_eq_arg)
{
    st_dict_id_t hash_key;
    st_dict_id_t ret;
    st_dict_node_t *work;

    ST_CHECK_PARAM(pnode == NULL 
            || (pnode->sign1 == 0 && pnode->sign2 == 0), -1);

    if(st_dict_seek(wd, pnode, node_eq_arg)== 0)
    {
        ST_WARNING("node already exists");
        return -1;
    }

    hash_key = wd->hash_func(wd, pnode);
    work = wd->first_level_node + hash_key;
    if(work->sign1 == 0 && work->sign2 == 0)
    {
        work->sign1 = pnode->sign1;
        work->sign2 = pnode->sign2;
        work->uint1 = pnode->uint1;
        work->next = ST_DICT_BAD_NODE;

        if(wd->clear_nodes != NULL)
        {
            wd->clear_nodes[wd->clear_node_num++] = hash_key;
        }
    }
    else
    {
        ret = st_dict_add_in(wd, pnode);
        if(ret == ST_DICT_BAD_NODE)
        {
            ST_WARNING("Failed to add in node");
            return -1;
        }
        wd->node_pool[ret].next = work->next;
        work->next = ret;
    }
    wd->node_num++;

    return 0;
}

int st_dict_add_no_seek(st_dict_t *wd, st_dict_node_t *pnode)
{
    st_dict_id_t hash_key;
    st_dict_id_t ret;
    st_dict_node_t *work;

    ST_CHECK_PARAM(pnode == NULL 
            || (pnode->sign1 == 0 && pnode->sign2 == 0), -1);

    hash_key = wd->hash_func(wd, pnode);
    work = wd->first_level_node + hash_key;
    if(work->sign1 == 0 && work->sign2 == 0)
    {
        work->sign1 = pnode->sign1;
        work->sign2 = pnode->sign2;
        work->uint1 = pnode->uint1;
        work->next = ST_DICT_BAD_NODE;
        if(wd->clear_nodes != NULL)
        {
            wd->clear_nodes[wd->clear_node_num++] = hash_key;
        }
    }
    else
    {
        ret = st_dict_add_in(wd, pnode);
        if(ret == ST_DICT_BAD_NODE)
        {
            ST_WARNING("Failed to add in node");
            return -1;
        }
        wd->node_pool[ret].next = work->next;
        work->next = ret;
    }
    wd->node_num++;

    return 0;
}

int st_dict_seek(st_dict_t *wd, st_dict_node_t *pnode, void *node_eq_arg)
{
    st_dict_id_t hash_key;
    st_dict_node_t *work;

    ST_CHECK_PARAM(pnode == NULL 
            || (pnode->sign1 == 0 && pnode->sign2 == 0), -1);

    hash_key = wd->hash_func(wd, pnode);
    work = wd->first_level_node + hash_key;
    if(work->sign1 == 0 && work->sign2 == 0)
    {
        return -1;
    }

    if(wd->node_eq_func(work, pnode, node_eq_arg))
    {
        pnode->uint1 = work->uint1;
        return 0;
    }

    while(work->next != ST_DICT_BAD_NODE)
    {
        if(work->next >= wd->cur_index)
        {
            ST_WARNING("illegal next[%u/%u]", work->next, wd->cur_index);
            return -1;
        }
        work = wd->node_pool + work->next;
        if(wd->node_eq_func(work, pnode, node_eq_arg))
        {
            pnode->uint1 = work->uint1;
            return 0;
        }
    }
    
    return -1;
}

int st_dict_save(st_dict_t *wd, FILE *fp)
{
    size_t ret = 0;

    ST_CHECK_PARAM(wd == NULL || fp == NULL, -1);

    ret = fwrite(&wd->hash_num, sizeof(st_dict_id_t), 1, fp);
    if(ret != 1)
    {
        ST_WARNING("Failed to write hash_num");
        return -1;
    }

    ret = fwrite(&wd->realloc_node_num, sizeof(st_dict_id_t), 1, fp);
    if(ret != 1)
    {
        ST_WARNING("Failed to write realloc_node_num");
        return -1;
    }

    ret = fwrite(&wd->cur_index, sizeof(st_dict_id_t), 1, fp);
    if(ret != 1)
    {
        ST_WARNING("Failed to write cur_index");
        return -1;
    }

    ret = fwrite(&wd->max_pool_num, sizeof(st_dict_id_t), 1, fp);
    if(ret != 1)
    {
        ST_WARNING("Failed to write max_pool_num");
        return -1;
    }

    ret = fwrite(&wd->node_num, sizeof(st_dict_id_t), 1, fp);
    if(ret != 1)
    {
        ST_WARNING("Failed to write node_num");
        return -1;
    }

    ret = fwrite(&wd->addr_mask, sizeof(st_dict_id_t), 1, fp);
    if(ret != 1)
    {
        ST_WARNING("Failed to write addr_mask");
        return -1;
    }

    ret = fwrite(wd->first_level_node, sizeof(st_dict_node_t),
        wd->hash_num, fp);
    if(ret != (size_t)wd->hash_num)
    {
        ST_WARNING("Failed to write first_level_node");
        return -1;
    }
    
    ret = fwrite(wd->node_pool, sizeof(st_dict_node_t),
        wd->max_pool_num, fp);
    if(ret != (size_t)wd->max_pool_num)
    {
        ST_WARNING("Failed to write node_pool");
        return -1;
    }
    
    fflush(fp);

    return 0;
}

int st_dict_load(st_dict_t *wd, FILE *fp)
{
    size_t ret = 0;

    ST_CHECK_PARAM(wd == NULL || fp == NULL, -1);

    ret = fread(&wd->hash_num, sizeof(st_dict_id_t), 1, fp);
    if(ret != 1)
    {
        ST_WARNING("Failed to read hash_num");
        return -1;
    }

    ret = fread(&wd->realloc_node_num, sizeof(st_dict_id_t), 1, fp);
    if(ret != 1)
    {
        ST_WARNING("Failed to read realloc_node_num");
        return -1;
    }

    ret = fread(&wd->cur_index, sizeof(st_dict_id_t), 1, fp);
    if(ret != 1)
    {
        ST_WARNING("Failed to read cur_index");
        return -1;
    }

    ret = fread(&wd->max_pool_num, sizeof(st_dict_id_t), 1, fp);
    if(ret != 1)
    {
        ST_WARNING("Failed to read max_pool_num");
        return -1;
    }

    ret = fread(&wd->node_num, sizeof(st_dict_id_t), 1, fp);
    if(ret != 1)
    {
        ST_WARNING("Failed to read node_num");
        return -1;
    }

    ret = fread(&wd->addr_mask, sizeof(st_dict_id_t), 1, fp);
    if(ret != 1)
    {
        ST_WARNING("Failed to read addr_mask");
        return -1;
    }

    wd->first_level_node = (st_dict_node_t *)
        malloc(sizeof(st_dict_node_t)*wd->hash_num);
    if(wd->first_level_node == NULL)
    {
        ST_WARNING("Failed to alloc first_level_node.");
        return -1;
    }

    wd->node_pool = (st_dict_node_t *)
        malloc(sizeof(st_dict_node_t)*wd->max_pool_num);
    if(wd->node_pool == NULL)
    {
        ST_WARNING("Failed to alloc node_pool.");
        return -1;
    }

    ret = fread(wd->first_level_node, sizeof(st_dict_node_t),
        wd->hash_num, fp);
    if(ret != wd->hash_num)
    {
        ST_WARNING("Failed to read first_level_node");
        return -1;
    }
    
    ret = fread(wd->node_pool, sizeof(st_dict_node_t),
        wd->max_pool_num, fp);
    if(ret != wd->max_pool_num)
    {
        ST_WARNING("Failed to read node_pool");
        return -1;
    }
    
    return 0;
}

st_dict_t* st_dict_load_from_bin(FILE *fp)
{
    st_dict_t *wd;

    ST_CHECK_PARAM(fp == NULL, NULL);

    if((wd = st_dict_alloc()) == NULL)
    {
        ST_WARNING("Failed to st_dict_alloc.");
        return NULL;
    }

    if(st_dict_load(wd, fp) < 0)
    {
        ST_WARNING("Failed to st_alphabet_load_txt_fp.");
        goto ERR;
        
    }

    wd->hash_func = st_dict_hash_simple;
    wd->node_eq_func = st_dict_node_equal;

    return wd;
ERR:
    safe_st_dict_destroy(wd);
    return NULL;
}

int st_dict_traverse(st_dict_t *wd, st_dict_trav_func_t trav, void *args)
{
    st_dict_node_t *work;
    st_dict_node_t *first_level_node;
    st_dict_node_t *node_pool;
    st_dict_id_t id;
    st_dict_id_t did;

    ST_CHECK_PARAM(wd == NULL, -1);

    first_level_node = wd->first_level_node;
    node_pool = wd->node_pool;

    for(id = 0; id < wd->hash_num; id++) {
        work = first_level_node + id;

        if (work->sign1 == 0 && work->sign2 == 0) {
            continue;
        }

        if(trav != NULL && trav(work, args) < 0) {
            ST_WARNING("Failed to trav.");
            return -1;
        }

        did = work->next;
        while(did != ST_DICT_BAD_NODE) {
            if(did >= wd->cur_index) {
                ST_WARNING("illegal next");
                return -1;
            }

            work = node_pool + did;
            did = work->next;

            assert(work->sign1 != 0 && work->sign2 != 0);

            if(trav != NULL && trav(work, args) < 0) {
                ST_WARNING("Failed to trav.");
                return -1;
            }
        }
    }

    return 0;
}

int st_dict_clear(st_dict_t *wd, st_dict_trav_func_t trav, void *args)
{
    st_dict_node_t *work;
    st_dict_node_t *first_level_node;
    st_dict_node_t *node_pool;
    st_dict_id_t   *clear_nodes;
    st_dict_id_t id;
    st_dict_id_t did;
    st_dict_id_t clear_node_num;

    ST_CHECK_PARAM(wd == NULL || wd->clear_nodes == NULL, -1);

    first_level_node = wd->first_level_node;
    node_pool = wd->node_pool;
    clear_nodes = wd->clear_nodes;
    clear_node_num = wd->clear_node_num;

    for(id = 0; id < clear_node_num; id++)
    {
        work = first_level_node + clear_nodes[id];

        assert(work->sign1 != 0 || work->sign2 != 0);

        if(trav != NULL && trav(work, args) < 0)
        {
            ST_WARNING("Failed to trav.");
            return -1;
        }
        wd->node_num--;

        work->sign1 = 0;
        work->sign2 = 0;
        work->uint1 = 0;
        did = work->next;
        while(did != ST_DICT_BAD_NODE)
        {
            if(did >= wd->cur_index)
            {
                ST_WARNING("illegal next");
                return -1;
            }

            work = node_pool + did;
            did = work->next;

            assert(work->sign1 != 0 && work->sign2 != 0);

            if(trav != NULL && trav(work, args) < 0)
            {
                ST_WARNING("Failed to trav.");
                return -1;
            }

            wd->node_num--;
            work->sign1 = 0;
            work->sign2 = 0;
            work->uint1 = 0;
            work->next = ST_DICT_BAD_NODE;
        }
    }

    wd->clear_node_num = 0;

    return 0;
}

int st_dict_update(st_dict_t *wd, st_dict_node_t *pnode, void *node_eq_arg,
        st_dict_update_func_t update_data)
{
    st_dict_id_t hash_key;
    st_dict_id_t ret;
    st_dict_node_t *work;

    ST_CHECK_PARAM(pnode == NULL 
            || (pnode->sign1 == 0 && pnode->sign2 == 0), -1);

    hash_key = wd->hash_func(wd, pnode);
    work = wd->first_level_node + hash_key;
    if(wd->node_eq_func(work, pnode, node_eq_arg))
    {
        if(update_data(work, pnode->float1) < 0)
        {
            ST_WARNING("Failed to update_data.");
            return -1;
        }
        return 0;
    }

    while(work->next != ST_DICT_BAD_NODE)
    {
        if(work->next >= wd->cur_index)
        {
            ST_WARNING("illegal next");
            return -1;
        }
        work = wd->node_pool + work->next;
        if(wd->node_eq_func(work, pnode, node_eq_arg))
        {
            if(update_data(work, pnode->float1) < 0)
            {
                ST_WARNING("Failed to update_data.");
                return -1;
            }
            return 0;
        }
    }
    
    work = wd->first_level_node + hash_key;
    if(work->sign1 == 0 && work->sign2 == 0)
    {
        work->sign1 = pnode->sign1;
        work->sign2 = pnode->sign2;
        work->uint1 = pnode->uint1;
        work->next = ST_DICT_BAD_NODE;

        if(wd->clear_nodes != NULL)
        {
            wd->clear_nodes[wd->clear_node_num++] = hash_key;
        }
    }
    else
    {
        ret = st_dict_add_in(wd, pnode);
        if(ret == ST_DICT_BAD_NODE)
        {
            ST_WARNING("Failed to add in node");
            return -1;
        }
        wd->node_pool[ret].next = work->next;
        work->next = ret;
    }
    wd->node_num++;
    return 0;
}

st_dict_t* st_dict_dup(st_dict_t *d)
{
    st_dict_t *dict = NULL;

    ST_CHECK_PARAM(d == NULL, NULL);

    dict = (st_dict_t *)malloc(sizeof(st_dict_t));
    if(dict == NULL) {
        ST_WARNING("Failed to alloc mem for st_dict.");
        return NULL;
    }
    memset(dict, 0, sizeof(st_dict_t));

    dict->hash_num = d->hash_num;
    dict->realloc_node_num = d->realloc_node_num;
    dict->addr_mask = d->addr_mask;
    dict->cur_index = d->cur_index;
    dict->max_pool_num = d->max_pool_num;
    dict->node_num = d->node_num;
    dict->clear_node_num = d->clear_node_num;

    dict->hash_func = d->hash_func;
    dict->node_eq_func = d->node_eq_func;

    dict->first_level_node = (st_dict_node_t *)
        malloc(sizeof(st_dict_node_t) * dict->hash_num);
    if(dict->first_level_node == NULL) {
        ST_WARNING("Failed to alloc mem for first_level_node.");
        goto ERR;
    }
    memcpy(dict->first_level_node, d->first_level_node,
            sizeof(st_dict_node_t)*dict->hash_num);

    dict->node_pool = (st_dict_node_t *)
        malloc(sizeof(st_dict_node_t)*dict->max_pool_num);
    if(dict->node_pool == NULL) {
        ST_WARNING("Failed to alloc mem for node_pool.");
        goto ERR;
    }

    memcpy(dict->node_pool, d->node_pool,
            sizeof(st_dict_node_t)*dict->max_pool_num);

    if(d->clear_nodes != NULL) {
        dict->clear_nodes = (st_dict_id_t *)
            malloc(sizeof(st_dict_id_t)*dict->hash_num);
        if(dict->clear_nodes == NULL) {
            ST_WARNING("Failed to alloc mem for clear_nodes.");
            goto ERR;
        }
        memcpy(dict->clear_nodes, d->clear_nodes,
                sizeof(st_dict_id_t)*dict->hash_num);
    }

    return dict;

ERR:
    safe_st_dict_destroy(dict);
    return NULL;
}

