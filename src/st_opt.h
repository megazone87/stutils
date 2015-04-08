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

#ifndef _ST_OPT_H_
#define _ST_OPT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "st_macro.h"
#include "st_conf.h"

typedef enum _st_opt_type_t_ {
    SOT_BOOL,
    SOT_INT,
    SOT_UINT,
    SOT_LONG,
    SOT_ULONG,
    SOT_FLOAT,
    SOT_STR,
} st_opt_type_t;

typedef struct _st_opt_info_t_ {
    char sec_name[MAX_ST_CONF_LEN];
    char name[MAX_ST_CONF_LEN];
    char desc[MAX_LINE_LEN];
    st_opt_type_t type;
    union {
        bool bval;
        int ival;
        uint uval;
        float fval;
        long lval;
        unsigned long ulval;
        char sval[MAX_ST_CONF_LEN];
    };
} st_opt_info_t;

typedef struct _st_opt_t_ {
    st_conf_t *file_conf;
    st_conf_t *cmd_conf;

    st_opt_info_t *infos;
    int info_num;
    int info_cap;
} st_opt_t;

st_opt_t *st_opt_create();

int st_opt_parse(st_opt_t *opt, int *argc, const char *argv[]);

#define safe_st_opt_destroy(ptr) do {\
    if((ptr) != NULL) {\
        st_opt_destroy(ptr);\
        safe_free(ptr);\
        (ptr) = NULL;\
    }\
    } while(0)
void st_opt_destroy(st_opt_t *popt);

void st_opt_show(st_opt_t *popt, const char *header);

void st_opt_show_usage(st_opt_t *opt, FILE *fp);

int st_opt_get_float(st_opt_t *popt, const char *sec_name,
        const char *key, float *value, float default_value,
        const char *desc);

int st_opt_get_int(st_opt_t *popt, const char *sec_name,
        const char *key, int *value, int default_value,
        const char *desc);

int st_opt_get_uint(st_opt_t *popt, const char *sec_name,
        const char *key, unsigned int *value, unsigned int default_value,
        const char *desc);

int st_opt_get_long(st_opt_t *popt, const char *sec_name,
        const char *key, long *value, long default_value,
        const char *desc);

int st_opt_get_ulong(st_opt_t *popt, const char *sec_name,
        const char *key, unsigned long *value, unsigned long default_value,
        const char *desc);

int st_opt_get_str(st_opt_t *popt, const char *sec_name,
        const char *key, char *value, int vlen, const char *default_value,
        const char *desc);

int st_opt_get_bool(st_opt_t *popt, const char *sec_name,
        const char *key, bool *value, const bool default_value,
        const char *desc);

#define ST_OPT_GET_BOOL(pconf, key, var, def, desc) \
    do{\
        if (st_opt_get_bool(pconf, NULL, key, &var, def, desc) < 0) {\
            ST_WARNING("Failed to getopt key[%s] "\
                    "in section[" DEF_SEC_NAME "].", key);\
            goto ST_OPT_ERR; \
        }\
    } while(0)

#define ST_OPT_SEC_GET_BOOL(pconf, sec, key, var, def, desc) \
    do {\
        if (st_opt_get_bool(pconf, sec, key, &var, def, desc) < 0) {\
            ST_WARNING("Failed to getopt key[%s] "\
                    "in section[%s].", key, sec);\
            goto ST_OPT_ERR; \
        }\
    } while(0)

#define ST_OPT_GET_INT(pconf, key, var, def, desc) \
    do{\
        if (st_opt_get_int(pconf, NULL, key, &var, def, desc) < 0) {\
            ST_WARNING("Failed to getopt key[%s] "\
                    "in section[" DEF_SEC_NAME "].", key);\
            goto ST_OPT_ERR; \
        }\
    } while(0)

#define ST_OPT_SEC_GET_INT(pconf, sec, key, var, def, desc) \
    do{\
        if (st_opt_get_int(pconf, sec, key, &var, def, desc) < 0) {\
            ST_WARNING("Failed to getopt key[%s] "\
                    "in section[%s].", key, sec);\
            goto ST_OPT_ERR; \
        }\
    } while(0)

#define ST_OPT_GET_UINT(pconf, key, var, def, desc) \
    do{\
        if (st_opt_get_uint(pconf, NULL, key, &var, def, desc) < 0) {\
            ST_WARNING("Failed to getopt key[%s] "\
                    "in section[" DEF_SEC_NAME "].", key);\
            goto ST_OPT_ERR; \
        }\
    } while(0)

#define ST_OPT_SEC_GET_UINT(pconf, sec, key, var, def, desc) \
    do{\
        if (st_opt_get_uint(pconf, sec, key, &var, def, desc) < 0) {\
            ST_WARNING("Failed to getopt key[%s] "\
                    "in section[%s].", key, sec);\
            goto ST_OPT_ERR; \
        }\
    } while(0)

#define ST_OPT_GET_LONG(pconf, key, var, def, desc) \
    do{\
        if (st_opt_get_long(pconf, NULL, key, &var, def, desc) < 0) {\
            ST_WARNING("Failed to getopt key[%s] "\
                    "in section[" DEF_SEC_NAME "].", key);\
            goto ST_OPT_ERR; \
        }\
    } while(0)

#define ST_OPT_SEC_GET_LONG(pconf, sec, key, var, def, desc) \
    do{\
        if (st_opt_get_long(pconf, sec, key, &var, def, desc) < 0) {\
            ST_WARNING("Failed to getopt key[%s] "\
                    "in section[%s].", key, sec);\
            goto ST_OPT_ERR; \
        }\
    } while(0)

#define ST_OPT_GET_ULONG(pconf, key, var, def, desc) \
    do{\
        if (st_opt_get_ulong(pconf, NULL, key, &var, def, desc) < 0) {\
            ST_WARNING("Failed to getopt key[%s] "\
                    "in section[" DEF_SEC_NAME "].", key);\
            goto ST_OPT_ERR; \
        }\
    } while(0)

#define ST_OPT_SEC_GET_ULONG(pconf, sec, key, var, def, desc) \
    do{\
        if (st_opt_get_ulong(pconf, sec, key, &var, def, desc) < 0) {\
            ST_WARNING("Failed to getopt key[%s] "\
                    "in section[%s].", key, sec);\
            goto ST_OPT_ERR; \
        }\
    } while(0)

#define ST_OPT_GET_STR(pconf, key, var, len, def, desc) \
    do{\
        if (st_opt_get_str(pconf, NULL, key, var, len, def, desc) < 0) {\
            ST_WARNING("Failed to getopt key[%s] "\
                    "in section[" DEF_SEC_NAME "].", key);\
            goto ST_OPT_ERR; \
        }\
    } while(0)

#define ST_OPT_SEC_GET_STR(pconf, sec, key, var, len, def, desc) \
    do{\
        if (st_opt_get_str(pconf, sec, key, var, len, def, desc) < 0) {\
            ST_WARNING("Failed to getopt key[%s] "\
                    "in section[%s].", key, sec);\
            goto ST_OPT_ERR; \
        }\
    } while(0)

#define ST_OPT_GET_FLOAT(pconf, key, var, def, desc) \
    do{\
        if (st_opt_get_float(pconf, NULL, key, &var, def, desc) < 0) {\
            ST_WARNING("Failed to getopt key[%s] "\
                    "in section[" DEF_SEC_NAME "].", key);\
            goto ST_OPT_ERR; \
    } while(0)

#define ST_OPT_SEC_GET_FLOAT(pconf, sec, key, var, def, desc) \
    do{\
        if (st_opt_get_float(pconf, sec, key, &var, def, desc) < 0) {\
            ST_WARNING("Failed to getopt key[%s] "\
                    "in section[%s].", key, sec);\
            goto ST_OPT_ERR; \
        }\
    } while(0)

#ifdef __cplusplus
}
#endif

#endif

