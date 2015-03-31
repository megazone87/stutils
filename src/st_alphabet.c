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
#include "st_utils.h"
#include "st_alphabet.h"

#define SYM_NUM "symbols"


void get_sign(const char *psrc, int slen, st_dict_sign_t *sign1, st_dict_sign_t *sign2)
{
    *sign1=0;
    *sign2=0;
    if( slen <= 4 ) {
        memcpy(sign1,psrc,slen);
    } else {
        if(slen<=8)
        {
            memcpy(sign1,psrc,4);
            memcpy(sign2,psrc+4,slen-4);
        }
        else
        {
            *sign1 = MurmurHash2 ( psrc, slen / 2, 1 );
            *sign2 = MurmurHash2 ( psrc + slen / 2, slen - slen / 2, 2 );
        }
    }
}

void st_alphabet_destroy(st_alphabet_t *alphabet)
{
    if (alphabet == NULL) {
        return;
    }

    if(alphabet->labels) {
        safe_free(alphabet->labels);
    }

    if(alphabet->is_aux) {
        safe_free(alphabet->is_aux);
    }

    if(alphabet->index_dict) {
        safe_st_dict_destroy(alphabet->index_dict);
    }
}

static st_alphabet_t* st_alphabet_alloc()
{
    st_alphabet_t *alphabet;
    
    alphabet = (st_alphabet_t *)malloc(sizeof(st_alphabet_t));
    if(alphabet == NULL)
    {
        ST_WARNING("Failed to alloc alphabet.");
        return NULL;
    }

    alphabet->labels = NULL;
    alphabet->is_aux = NULL;
    alphabet->label_num = 0;
    alphabet->aux_num = 0;
    alphabet->index_dict = NULL;

    return alphabet;
}

typedef struct _index_dict_eq_args_t_ {
    st_alphabet_t *alphabet;
    const char *label;
} index_dict_eq_args_t;

static bool index_dict_node_eq(st_dict_node_t *node1,
    st_dict_node_t *node2, void *args)
{
    index_dict_eq_args_t *arg;
    st_alphabet_t *alphabet;

    if (node1->sign1 != node2->sign1 || node1->sign2 != node2->sign2) {
        return false;
    }

    arg = (index_dict_eq_args_t *)args;
    alphabet = arg->alphabet;

    if (alphabet == NULL) {
        ST_WARNING("alphabet == NULL.");
        return false;
    }

    if (node1->uint1 >= alphabet->label_num) {
        ST_WARNING("node->uint1 overflow[%u/%u].", node1->uint1, alphabet->label_num);
        return false;
    }

    return (strncmp(alphabet->labels[node1->uint1].label, arg->label, MAX_SYM_LEN) == 0);
}

st_alphabet_t* st_alphabet_create(int max_label_num)
{
    st_alphabet_t *alphabet = NULL;
    int i;

    ST_CHECK_PARAM(max_label_num <= 0, NULL);

    alphabet = st_alphabet_alloc();
    if(alphabet == NULL)
    {
        ST_WARNING("Failed to alphabet_alloc.");
        goto ERR;
    }

    alphabet->max_label_num = max_label_num;
    alphabet->labels = (st_label_t *)
        malloc(max_label_num * sizeof(st_label_t));
    if(alphabet->labels == NULL)
    {
        ST_WARNING("Failed to allocate memory for labels.");
        goto ERR;
    }
    
    for(i = 0; i < max_label_num; i++)
    {
        alphabet->labels[i].symid = -1;
        alphabet->labels[i].label[0] = 0;
    }
    
    if((alphabet->index_dict = st_dict_create(max_label_num,
        ST_DICT_REALLOC_NUM, NULL, index_dict_node_eq, false)) == NULL)
    {
        ST_WARNING("Failed to alloc index_dict");
        goto ERR;
    }

    alphabet->is_aux = (bool *) malloc(max_label_num * sizeof(bool));
    if(alphabet->is_aux == NULL)
    {
        ST_WARNING("Failed to allocate memory for is_aux.");
        goto ERR;
    }
    memset(alphabet->is_aux, 0, max_label_num * sizeof(bool));
    
    return alphabet;

ERR:
    safe_st_alphabet_destroy(alphabet);
    return NULL;
}

int st_alphabet_add_label(st_alphabet_t *alphabet, const char *label_)
{
    st_dict_node_t snode;
    int ret = 0;

    if((ret = st_alphabet_get_index(alphabet, label_)) >= 0)
    {
        return ret;
    }

    if(alphabet->max_label_num <= alphabet->label_num)
    {
        ST_WARNING("label overflow[%d/%d]", alphabet->label_num, 
                alphabet->max_label_num);
        return -1;
    }

    alphabet->labels[alphabet->label_num].symid = alphabet->label_num;
    strncpy(alphabet->labels[alphabet->label_num].label, label_, MAX_SYM_LEN);
    alphabet->labels[alphabet->label_num].label[MAX_SYM_LEN - 1] = 0;

    get_sign((char *)label_, strlen(label_), &snode.sign1, &snode.sign2);
    snode.uint1 = alphabet->label_num;

    if(st_dict_add_no_seek(alphabet->index_dict, &snode) < 0)
    {
        ST_WARNING("Failed to add label[%s] into dict", label_);
        return -1;
    }

    if(label_[0] == '#')
    {
        alphabet->is_aux[alphabet->label_num] = true;
        alphabet->aux_num++;
    }

    alphabet->label_num++;
    return alphabet->label_num - 1;
}

int st_alphabet_get_label_num(st_alphabet_t *alphabet) 
{
    ST_CHECK_PARAM(alphabet == NULL, -1);

    return alphabet->label_num;
}

char *st_alphabet_get_label(st_alphabet_t *alphabet, int index) 
{
    ST_CHECK_PARAM_EX(alphabet == NULL || index < 0 
        || index > alphabet->label_num, NULL, "%d/%d", index,
        alphabet->label_num);

    return alphabet->labels[index].label;
}

int st_alphabet_get_index(st_alphabet_t *alphabet, const char *label)
{
    index_dict_eq_args_t arg;
    st_dict_node_t snode;

    ST_CHECK_PARAM(alphabet == NULL || label == NULL, -1);

    arg.alphabet = alphabet;
    arg.label = label;
    get_sign((char *)label, strlen(label), &snode.sign1, &snode.sign2);
    if(st_dict_seek(alphabet->index_dict, &snode, &arg) < 0)
    {
        return -1;
    }

    return (int)snode.uint1;
}

int st_alphabet_save_bin(st_alphabet_t *alphabet, FILE *fp)
{
    int ret = 0;

    ST_CHECK_PARAM(alphabet == NULL || fp == NULL, -1);

    ret = fwrite(&alphabet->label_num, sizeof(int), 1, fp);
    if(ret != 1) {
        ST_WARNING("Failed to write label_num");
        return -1;
    }
    
    ret = fwrite(&alphabet->aux_num, sizeof(int), 1, fp);
    if(ret != 1) {
        ST_WARNING("Failed to write aux_num");
        return -1;
    }
    
    ret = fwrite(alphabet->labels, sizeof(st_label_t),
        alphabet->label_num, fp);
    if(ret != alphabet->label_num) {
        ST_WARNING("Failed to write labels");
        return -1;
    }
    
    ret = fwrite(alphabet->is_aux, sizeof(bool), alphabet->label_num, fp);
    if(ret != alphabet->label_num) {
        ST_WARNING("Failed to write is_aux");
        return -1;
    }
    
    if(st_dict_save(alphabet->index_dict, fp) < 0) {
        ST_WARNING("Failed to save index_dict");
        return -1;
    }

    return 0;
}

int st_alphabet_save_txt(st_alphabet_t *alphabet, FILE *fp)
{
    st_label_t *labels;
    int i;

    ST_CHECK_PARAM(alphabet == NULL || fp == NULL, -1);

    labels = alphabet->labels;
    for(i = 0; i < alphabet->label_num; i++)
    {
        if(labels[i].symid != -1)
        {
            fprintf(fp, "%s\t%d\n", labels[i].label, labels[i].symid);
        }
    }

    return 0;
}

int st_alphabet_load_txt(st_alphabet_t *alphabet, FILE *fp)
{
    char line[MAX_LINE_LEN];
    char syms[MAX_SYM_LEN] ;
    st_dict_node_t snode;
    char *pstr = NULL;
    int id;
    int i;

    st_label_t *labels;
    bool *is_aux;
    st_dict_t *index_dict = NULL;
    int label_num;
    int aux_num = 0;

    ST_CHECK_PARAM(alphabet == NULL || fp == NULL, -1);

    if(fgets(line, MAX_LINE_LEN, fp) == NULL)
    {
        ST_WARNING("Empty file.");
        goto ERR;
    }
    pstr = strchr(line, '=');
    if(pstr == NULL || strncmp(line, SYM_NUM, strlen(SYM_NUM)) != 0)
    {
        ST_WARNING("Wrong esym format: no symbols num.");
        goto ERR;
    }
    label_num = atoi(pstr+1);
    if(label_num <= 0)
    {
        ST_WARNING("Wrong esym format: wrong symbols num[%d].", label_num);
        goto ERR;
    }

    labels = (st_label_t *)malloc(label_num * sizeof(st_label_t));
    if(labels == NULL)
    {
        ST_WARNING("Failed to allocate memory for labels.");
        goto ERR;
    }
    
    is_aux = (bool *)malloc(label_num * sizeof(bool));
    if(is_aux == NULL)
    {
        ST_WARNING("Failed to allocate memory for is_aux.");
        goto ERR;
    }

    for(i = 0; i < label_num; i++)
    {
        labels[i].symid = -1;
        labels[i].label[0] = 0;
        is_aux[i] = false;
    }
    
    if((index_dict = st_dict_create(label_num,
        ST_DICT_REALLOC_NUM, NULL, NULL, false)) == NULL)
    {
        ST_WARNING("Failed to alloc index_dict");
        goto ERR;
    }

    while(fgets(line, MAX_LINE_LEN, fp))
    {
        if(sscanf(line, "%s %d", syms, &id) != 2)
        {
            continue;
        }

        if(id >= label_num)
        {
            ST_WARNING("Wrong id[%d]>=label_num[%d].", id, label_num);
            goto ERR;
        }

        if(labels[id].symid != -1)
        {
            ST_WARNING("Replicated symbol [%d:%s].", id, syms);
            goto ERR;
        }

        strncpy(labels[id].label, syms, MAX_SYM_LEN);
        labels[id].label[MAX_SYM_LEN - 1] = 0;
        labels[id].symid = id;
        
        get_sign(labels[id].label, strlen(labels[id].label), 
                &snode.sign1, &snode.sign2);
        snode.uint1 = (uint)id;
        st_dict_add(index_dict, &snode, NULL);

        if(syms[0] == '#')
        {
            is_aux[id] = true;
            aux_num++;
        }
    }

    for(i = 0; i < label_num; i++)
    {
        if(labels[i].symid == -1)
        {
            ST_WARNING("Empty symbol for id[%d]", i);
            goto ERR;
        }
    }

    alphabet->labels = labels;
    alphabet->is_aux = is_aux;
    alphabet->max_label_num = label_num;
    alphabet->label_num = label_num;
    alphabet->aux_num = aux_num;
    alphabet->index_dict = index_dict;

    return 0;

ERR:
    safe_st_dict_destroy(index_dict);
    return -1;
}

st_alphabet_t* st_alphabet_load_from_txt(FILE *fp)
{
    st_alphabet_t *alphabet;

    ST_CHECK_PARAM(fp == NULL, NULL);

    if((alphabet = st_alphabet_alloc()) == NULL)
    {
        ST_WARNING("Failed to st_alphabet_alloc.");
        return NULL;
    }

    if(st_alphabet_load_txt(alphabet, fp) < 0)
    {
        ST_WARNING("Failed to st_alphabet_load_txt_fp.");
        goto ERR;
        
    }

    return alphabet;
ERR:
    safe_st_alphabet_destroy(alphabet);
    return NULL;
}

static int st_alphabet_load_bin(st_alphabet_t *alphabet, FILE *fp)
{
    int ret = 0;

    ST_CHECK_PARAM(alphabet == NULL || fp == NULL, -1);

    ret = fread(&alphabet->label_num, sizeof(int), 1, fp);
    if(ret != 1)
    {
        ST_WARNING("Failed to read label_num");
        return -1;
    }
    
    ret = fread(&alphabet->aux_num, sizeof(int), 1, fp);
    if(ret != 1)
    {
        ST_WARNING("Failed to read aux_num");
        return -1;
    }
    
    alphabet->labels = (st_label_t *)
        malloc(sizeof(st_label_t)*alphabet->label_num);
    if(alphabet->labels == NULL)
    {
        ST_WARNING("Failed to malloc labels. [%d]", alphabet->label_num);
        return -1;
    }

    alphabet->is_aux = (bool *)malloc(sizeof(bool)*alphabet->label_num);
    if(alphabet->is_aux == NULL)
    {
        ST_WARNING("Failed to malloc is_aux.");
        return -1;
    }

    ret = fread(alphabet->labels, sizeof(st_label_t),
        alphabet->label_num, fp);
    if(ret != alphabet->label_num)
    {
        ST_WARNING("Failed to read labels");
        return -1;
    }
    
    ret = fread(alphabet->is_aux, sizeof(bool), alphabet->label_num, fp);
    if(ret != alphabet->label_num)
    {
        ST_WARNING("Failed to read is_aux");
        return -1;
    }
    
    if((alphabet->index_dict = st_dict_load_from_bin(fp)) == NULL)
    {
        ST_WARNING("Failed to load index_dict");
        return -1;
    }

    return 0;
}

st_alphabet_t* st_alphabet_load_from_bin(FILE *fp)
{
    st_alphabet_t *alphabet;

    ST_CHECK_PARAM(fp == NULL, NULL);

    if((alphabet = st_alphabet_alloc()) == NULL)
    {
        ST_WARNING("Failed to st_alphabet_alloc.");
        return NULL;
    }

    if(st_alphabet_load_bin(alphabet, fp) < 0)
    {
        ST_WARNING("Failed to st_alphabet_load_bin_fp.");
        goto ERR;
        
    }

    return alphabet;
ERR:
    safe_st_alphabet_destroy(alphabet);
    return NULL;
}

st_alphabet_t* st_alphabet_dup(st_alphabet_t *a)
{
    st_alphabet_t *alphabet = NULL;

    ST_CHECK_PARAM(a == NULL, NULL);

    alphabet = st_alphabet_alloc();
    if(alphabet == NULL) {
        ST_WARNING("Failed to alphabet_alloc.");
        goto ERR;
    }

    alphabet->max_label_num = a->max_label_num;
    alphabet->label_num = a->label_num;
    alphabet->aux_num = a->aux_num;

    alphabet->labels = (st_label_t *)
        malloc(a->max_label_num * sizeof(st_label_t));
    if(alphabet->labels == NULL) {
        ST_WARNING("Failed to allocate memory for labels.");
        goto ERR;
    }
    
    memcpy(alphabet->labels, a->labels,
            sizeof(st_label_t)*a->max_label_num);
    
    alphabet->index_dict = st_dict_dup(a->index_dict);
    if (alphabet->index_dict == NULL) {
        ST_WARNING("Failed to st_dict_dup.");
        goto ERR;
    }

    alphabet->is_aux = (bool *) malloc(a->max_label_num * sizeof(bool));
    if(alphabet->is_aux == NULL) {
        ST_WARNING("Failed to allocate memory for is_aux.");
        goto ERR;
    }
    memcpy(alphabet->is_aux, a->is_aux, a->max_label_num*sizeof(bool));
    
    return alphabet;

ERR:
    safe_st_alphabet_destroy(alphabet);
    return NULL;
}

