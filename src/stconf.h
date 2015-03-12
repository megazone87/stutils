#ifndef _STCONF_H_
#define _STCONF_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_STCONF_LEN			    256
#define MAX_STCONF_LINE_LEN		    1024

typedef struct _stconf_param_t_
{
	char key[MAX_STCONF_LEN];
	char value[MAX_STCONF_LEN];
    int used;
} stconf_param_t;

typedef struct _stconf_section_t_
{
    char name[MAX_STCONF_LEN];

	stconf_param_t *param;
	int param_num;
    int param_cap;

	stconf_param_t *def_param;
	int def_param_num;
    int def_param_cap;

    int comment_out;
} stconf_section_t;

typedef struct _stconf_t_
{
	stconf_section_t *secs;
	int sec_num;
    int sec_cap;
} stconf_t;

stconf_t *init_stconf(const char *conf_file);

int resolve_param(const char *line, stconf_t *pconf, 
        stconf_section_t** sec);

int getfloatconf(stconf_t *pconf, const char *sec_name,
        const char *key, float *value, int *sec_i);

int getintconf(stconf_t * pconf, const char *sec_name,
        const char *key, int *value, int *sec_i);

int getuintconf(stconf_t *pconf, const char *sec_name,
        const char *key, unsigned int *value, int *sec_i);

int getstrconf(stconf_t * pconf, const char *sec_name,
        const char *key, char *value, int vlen, int *sec_i);

int getfloatconfdef(stconf_t *pconf, const char *sec_name,
        const char *key, float *value, float default_value);

int getintconfdef(stconf_t *pconf, const char *sec_name,
        const char *key, int *value, int default_value);

int getuintconfdef(stconf_t *pconf, const char *sec_name,
        const char *key, unsigned int *value, unsigned int default_value);

int getstrconfdef(stconf_t *pconf, const char *sec_name,
        const char *key, char *value, int vlen, const char *default_value);

void free_stconf(stconf_t *pconf);

void show_stconf(stconf_t *pconf, const char *header);

#define GET_INT_CONF(pconf, key, var) \
        do{\
        if(getintconf(pconf, NULL, #key, &var, NULL) < 0)\
        {\
            ST_WARNING("Failed to load key[" #key "] "\
                    "in section[__def_sec__].");\
            goto STCONF_ERR; \
        }\
        } while(0)

#define GET_INT_CONF_DEF(pconf, key, var, def) \
        do{\
            getintconfdef(pconf, NULL, #key, &var, def);\
        } while(0)

#define GET_INT_SEC_CONF(pconf, sec, key, var) \
        do{\
        if(getintconf(pconf, #sec, #key, &var, NULL) < 0)\
        {\
            ST_WARNING("Failed to load key[" #key "] "\
                    "in section[" #sec "].");\
            goto STCONF_ERR; \
        }\
        } while(0)

#define GET_INT_SEC_CONF_DEF(pconf, sec, key, var, def) \
        do{\
            getintconfdef(pconf, #sec, #key, &var, def);\
        } while(0)

#define GET_UINT_CONF(pconf, key, var) \
        do{\
        if(getuintconf(pconf, NULL, #key, &var, NULL) < 0)\
        {\
            ST_WARNING("Failed to load key[" #key "] "\
                    "in section[__def_sec__].");\
            goto STCONF_ERR; \
        }\
        } while(0)

#define GET_UINT_CONF_DEF(pconf, key, var, def) \
        do{\
            getuintconfdef(pconf, NULL, #key, &var, def);\
        } while(0)

#define GET_UINT_SEC_CONF(pconf, sec, key, var) \
        do{\
        if(getuintconf(pconf, #sec, #key, &var, NULL) < 0)\
        {\
            ST_WARNING("Failed to load key[" #key "] "\
                    "in section[" #sec "].");\
            goto STCONF_ERR; \
        }\
        } while(0)

#define GET_UINT_SEC_CONF_DEF(pconf, sec, key, var, def) \
        do{\
            getuintconfdef(pconf, #sec, #key, &var, def);\
        } while(0)

#define GET_STR_CONF(pconf, key, var, len) \
        do{\
            if(getstrconf(pconf, NULL, #key, var, len, NULL) < 0)\
            {\
                ST_WARNING("Failed to load key[" #key "] "\
                        "in section[__def_sec__].");\
                goto STCONF_ERR; \
            }\
        } while(0)

#define GET_STR_CONF_DEF(pconf, key, var, len, def) \
        do{\
            getstrconfdef(pconf, NULL, #key, var, len, def);\
        } while(0)

#define GET_STR_SEC_CONF(pconf, sec, key, var, len) \
        do{\
            if(getstrconf(pconf, #sec, #key, var, len, NULL) < 0)\
            {\
                ST_WARNING("Failed to load key[" #key "] "\
                        "in section[" #sec "].");\
                goto STCONF_ERR; \
            }\
        } while(0)

#define GET_STR_SEC_CONF_DEF(pconf, sec, key, var, len, def) \
        do{\
            getstrconfdef(pconf, #sec, #key, var, len, def);\
        } while(0)

#define GET_FLOAT_CONF(pconf, key, var) \
        do{\
        if(getfloatconf(pconf, NULL, #key, &var, NULL) < 0)\
        {\
            ST_WARNING("Failed to load key[" #key "] "\
                    "in section[__def_sec__].");\
            goto STCONF_ERR; \
        }\
        } while(0)

#define GET_FLOAT_CONF_DEF(pconf, key, var, def) \
        do{\
            getfloatconfdef(pconf, NULL, #key, &var, def);\
        } while(0)

#define GET_FLOAT_SEC_CONF(pconf, sec, key, var) \
        do{\
        if(getfloatconf(pconf, #sec, #key, &var, NULL) < 0)\
        {\
            ST_WARNING("Failed to load key[" #key "] "\
                    "in section[" #sec "].");\
            goto STCONF_ERR; \
        }\
        } while(0)

#define GET_FLOAT_SEC_CONF_DEF(pconf, sec, key, var, def) \
        do{\
            getfloatconfdef(pconf, #sec, #key, &var, def);\
        } while(0)

#ifdef __cplusplus
}
#endif

#endif

