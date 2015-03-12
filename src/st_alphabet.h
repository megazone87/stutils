#ifndef _ST_ALPHABET_H__
#define _ST_ALPHABET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "st_macro.h"
#include "st_dict.h"

#define MAX_SYM_LEN         256

typedef struct _st_label_t
{
    char label[MAX_SYM_LEN];
    int symid;
} st_label_t;

typedef struct _st_alphabet_t
{
    st_label_t *labels;
    int max_label_num;
    int label_num;

    bool *is_aux;
    int aux_num;

    st_dict_t *index_dict;
} st_alphabet_t;

st_alphabet_t* st_alphabet_load_from_txt(FILE *esym_fp);
st_alphabet_t* st_alphabet_load_from_bin(FILE *fp);

int st_alphabet_save_bin(st_alphabet_t *alphabet, FILE *fp);
int st_alphabet_save_txt(st_alphabet_t *alphabet, FILE *fp);

int st_alphabet_get_label_num(st_alphabet_t *alphabet);

char *st_alphabet_get_label(st_alphabet_t *alphabet, int index);
int st_alphabet_get_index(st_alphabet_t *alphabet, const char *label);

#define safe_st_alphabet_destroy(ptr) do {\
    if(ptr != NULL) {\
        st_alphabet_destroy(ptr);\
        safe_free(ptr);\
        ptr = NULL;\
    }\
    } while(0)

void st_alphabet_destroy(st_alphabet_t *alphabet);

st_alphabet_t* st_alphabet_create(int max_label_num);
int st_alphabet_add_label(st_alphabet_t *alphabet, const char *label_);


#ifdef __cplusplus
}
#endif

#endif

