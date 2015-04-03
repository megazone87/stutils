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

#ifndef _ST_CONF_H_
#define _ST_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "st_macro.h"

#define SEC_NUM     10
#define PARAM_NUM    100

#define MAX_ST_CONF_LEN        256
#define MAX_ST_CONF_LINE_LEN   1024

#define DEF_SEC_NAME "__def_sec__"

typedef struct _st_conf_param_t_
{
	char key[MAX_ST_CONF_LEN];
	char value[MAX_ST_CONF_LEN];
    int used;
} st_conf_param_t;

typedef struct _st_conf_section_t_
{
    char name[MAX_ST_CONF_LEN];

	st_conf_param_t *param;
	int param_num;
    int param_cap;

	st_conf_param_t *def_param;
	int def_param_num;
    int def_param_cap;

    int comment_out;
} st_conf_section_t;

typedef struct _st_conf_t_
{
	st_conf_section_t *secs;
	int sec_num;
    int sec_cap;
} st_conf_t;

st_conf_t* st_conf_create();

int st_conf_load(st_conf_t *st_conf, const char *conf_file);

#define safe_st_conf_destroy(ptr) do {\
    if((ptr) != NULL) {\
        st_conf_destroy(ptr);\
        safe_free(ptr);\
        (ptr) = NULL;\
    }\
    } while(0)
void st_conf_destroy(st_conf_t *pconf);

void st_conf_show(st_conf_t *pconf, const char *header);

st_conf_section_t* st_conf_new_sec(st_conf_t *conf, const char *name);

st_conf_section_t* st_conf_def_sec(st_conf_t *conf);

int st_resolve_param(const char *line, st_conf_t *pconf, 
        st_conf_section_t** sec);

int st_conf_add_param(st_conf_section_t *sec, const char *key, 
        const char *value);

int st_conf_get_float(st_conf_t *pconf, const char *sec_name,
        const char *key, float *value, int *sec_i);

int st_conf_get_int(st_conf_t * pconf, const char *sec_name,
        const char *key, int *value, int *sec_i);

int st_conf_get_uint(st_conf_t *pconf, const char *sec_name,
        const char *key, unsigned int *value, int *sec_i);

int st_conf_get_str(st_conf_t * pconf, const char *sec_name,
        const char *key, char *value, int vlen, int *sec_i);

int st_conf_get_bool(st_conf_t *pconf, const char *sec_name,
        const char *key, bool *value, int *sec_i);

int st_conf_get_float_def(st_conf_t *pconf, const char *sec_name,
        const char *key, float *value, float default_value);

int st_conf_get_int_def(st_conf_t *pconf, const char *sec_name,
        const char *key, int *value, int default_value);

int st_conf_get_uint_def(st_conf_t *pconf, const char *sec_name,
        const char *key, unsigned int *value, unsigned int default_value);

int st_conf_get_str_def(st_conf_t *pconf, const char *sec_name,
        const char *key, char *value, int vlen, const char *default_value);

int st_conf_get_bool_def(st_conf_t *pconf, const char *sec_name,
        const char *key, bool *value, const bool default_value);

#define ST_CONF_GET_BOOL(pconf, key, var) \
    do{\
        if (st_conf_get_bool(pconf, NULL, key, &var, NULL) < 0) {\
            ST_WARNING("Failed to load key[%s] "\
                    "in section[" DEF_SEC_NAME "].", key);\
            goto ST_CONF_ERR; \
        }\
    } while(0)

#define ST_CONF_GET_BOOL_DEF(pconf, key, var, def) \
    do{\
        if (st_conf_get_bool_def(pconf, NULL, key, &var, def) < 0) {\
            ST_WARNING("Failed to load key[%s] "\
                    "in section[" DEF_SEC_NAME "].", key);\
            goto ST_CONF_ERR; \
        }\
    } while(0)

#define ST_CONF_SEC_GET_BOOL(pconf, sec, key, var) \
    do {\
        if (st_conf_get_bool(pconf, sec, key, &var, NULL) < 0) {\
            ST_WARNING("Failed to load key[%s] "\
                    "in section[%s].", key, sec);\
            goto ST_CONF_ERR; \
        }\
    } while(0)

#define ST_CONF_SEC_GET_BOOL_DEF(pconf, sec, key, var, def) \
    do {\
        if (st_conf_get_bool_def(pconf, sec, key, &var, def) < 0) {\
            ST_WARNING("Failed to load key[%s] "\
                    "in section[%s].", key, sec);\
            goto ST_CONF_ERR; \
        }\
    } while(0)

#define ST_CONF_GET_INT(pconf, key, var) \
    do{\
        if (st_conf_get_int(pconf, NULL, key, &var, NULL) < 0) {\
            ST_WARNING("Failed to load key[%s] "\
                    "in section[" DEF_SEC_NAME "].", key);\
            goto ST_CONF_ERR; \
        }\
    } while(0)

#define ST_CONF_GET_INT_DEF(pconf, key, var, def) \
    do{\
        if (st_conf_get_int_def(pconf, NULL, key, &var, def) < 0) {\
            ST_WARNING("Failed to load key[%s] "\
                    "in section[" DEF_SEC_NAME "].", key);\
            goto ST_CONF_ERR; \
        }\
    } while(0)

#define ST_CONF_SEC_GET_INT(pconf, sec, key, var) \
    do{\
        if (st_conf_get_int(pconf, sec, key, &var, NULL) < 0) {\
            ST_WARNING("Failed to load key[%s] "\
                    "in section[%s].", key, sec);\
            goto ST_CONF_ERR; \
        }\
    } while(0)

#define ST_CONF_SEC_GET_INT_DEF(pconf, sec, key, var, def) \
    do{\
        if (st_conf_get_int_def(pconf, sec, key, &var, def) < 0) {\
            ST_WARNING("Failed to load key[%s] "\
                    "in section[%s].", key, sec);\
            goto ST_CONF_ERR; \
        }\
    } while(0)

#define ST_CONF_GET_UINT(pconf, key, var) \
    do{\
        if (st_conf_get_uint(pconf, NULL, key, &var, NULL) < 0) {\
            ST_WARNING("Failed to load key[%s] "\
                    "in section[" DEF_SEC_NAME "].", key);\
            goto ST_CONF_ERR; \
        }\
    } while(0)

#define ST_CONF_GET_UINT_DEF(pconf, key, var, def) \
    do{\
        if (st_conf_get_uint_def(pconf, NULL, key, &var, def) < 0) {\
            ST_WARNING("Failed to load key[%s] "\
                    "in section[" DEF_SEC_NAME "].", key);\
            goto ST_CONF_ERR; \
        }\
    } while(0)

#define ST_CONF_SEC_GET_UINT(pconf, sec, key, var) \
    do{\
        if (st_conf_get_uint(pconf, sec, key, &var, NULL) < 0) {\
            ST_WARNING("Failed to load key[%s] "\
                    "in section[%s].", key, sec);\
            goto ST_CONF_ERR; \
        }\
    } while(0)

#define ST_CONF_SEC_GET_UINT_DEF(pconf, sec, key, var, def) \
    do{\
        if (st_conf_get_uint_def(pconf, sec, key, &var, def) < 0) {\
            ST_WARNING("Failed to load key[%s] "\
                    "in section[%s].", key, sec);\
            goto ST_CONF_ERR; \
        }\
    } while(0)

#define ST_CONF_GET_STR(pconf, key, var, len) \
    do{\
        if (st_conf_get_str(pconf, NULL, key, var, len, NULL) < 0) {\
            ST_WARNING("Failed to load key[%s] "\
                    "in section[" DEF_SEC_NAME "].", key);\
            goto ST_CONF_ERR; \
        }\
    } while(0)

#define ST_CONF_GET_STR_DEF(pconf, key, var, len, def) \
    do{\
        if (st_conf_get_str_def(pconf, NULL, key, var, len, def) < 0) {\
            ST_WARNING("Failed to load key[%s] "\
                    "in section[" DEF_SEC_NAME "].", key);\
            goto ST_CONF_ERR; \
        }\
    } while(0)

#define ST_CONF_SEC_GET_STR(pconf, sec, key, var, len) \
    do{\
        if (st_conf_get_str(pconf, sec, key, var, len, NULL) < 0) {\
            ST_WARNING("Failed to load key[%s] "\
                    "in section[%s].", key, sec);\
            goto ST_CONF_ERR; \
        }\
    } while(0)

#define ST_CONF_SEC_GET_STR_DEF(pconf, sec, key, var, len, def) \
    do{\
        if (st_conf_get_str_def(pconf, sec, key, var, len, def) < 0) {\
            ST_WARNING("Failed to load key[%s] "\
                    "in section[%s].", key, sec);\
            goto ST_CONF_ERR; \
        }\
    } while(0)

#define ST_CONF_GET_FLOAT(pconf, key, var) \
    do{\
        if (st_conf_get_float(pconf, NULL, key, &var, NULL) < 0) {\
            ST_WARNING("Failed to load key[%s] "\
                    "in section[" DEF_SEC_NAME "].", key);\
            goto ST_CONF_ERR; \
        }\
    } while(0)

#define ST_CONF_GET_FLOAT_DEF(pconf, key, var, def) \
    do{\
        if (st_conf_get_float_def(pconf, NULL, key, &var, def) < 0) {\
            ST_WARNING("Failed to load key[%s] "\
                    "in section[" DEF_SEC_NAME "].", key);\
            goto ST_CONF_ERR; \
    } while(0)

#define ST_CONF_SEC_GET_FLOAT(pconf, sec, key, var) \
    do{\
        if (st_conf_get_float(pconf, sec, key, &var, NULL) < 0) {\
            ST_WARNING("Failed to load key[%s] "\
                    "in section[%s].", key, sec);\
            goto ST_CONF_ERR; \
        }\
    } while(0)

#define ST_CONF_SEC_GET_FLOAT_DEF(pconf, sec, key, var, def) \
    do{\
        if (st_conf_get_float_def(pconf, sec, key, &var, def) < 0) {\
            ST_WARNING("Failed to load key[%s] "\
                    "in section[%s].", key, sec);\
            goto ST_CONF_ERR; \
        }\
    } while(0)

#ifdef __cplusplus
}
#endif

#endif

