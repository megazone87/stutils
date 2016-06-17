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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "st_log.h"
#include "st_conf.h"

#define SEC_NUM     10
#define PARAM_NUM    100

static int resize_sec(st_conf_section_t *sec)
{
    if (sec->param_num >= sec->param_cap) {
        sec->param_cap += PARAM_NUM;
        sec->param = (st_conf_param_t *) realloc(sec->param,
                    sec->param_cap * sizeof(st_conf_param_t));
        if (sec->param == NULL) {
            ST_WARNING("Failed to realloc param for sec.");
            goto ERR;
        }
        memset(sec->param + sec->param_num, 0,
                PARAM_NUM * sizeof(st_conf_param_t));
    }

    if (sec->def_param_num >= sec->def_param_cap) {
        sec->def_param_cap += PARAM_NUM;
        sec->def_param = (st_conf_param_t *) realloc(sec->def_param,
                    sec->def_param_cap * sizeof(st_conf_param_t));
        if (sec->def_param == NULL) {
            ST_WARNING("Failed to realloc def_param for sec.");
            goto ERR;
        }
        memset(sec->def_param + sec->def_param_num, 0,
                PARAM_NUM * sizeof(st_conf_param_t));
    }

    return 0;
ERR:
    return -1;
}

st_conf_section_t* st_conf_new_sec(st_conf_t *conf, const char *name)
{
    int s;

    for (s = 0; s < conf->sec_num; s++) {
        if (strcasecmp(conf->secs[s].name, name) == 0) {
            return conf->secs + s;
        }
    }

    if (conf->sec_num >= conf->sec_cap) {
        conf->sec_cap += SEC_NUM;
        conf->secs = (st_conf_section_t *) realloc(conf->secs,
                    conf->sec_cap * sizeof(st_conf_section_t));
        if (conf->secs == NULL) {
            ST_WARNING("Failed to realloc secs.");
            goto ERR;
        }
        memset(conf->secs + conf->sec_num, 0,
                SEC_NUM * sizeof(st_conf_section_t));
    }

    strncpy(conf->secs[conf->sec_num].name, name, MAX_ST_CONF_LEN);
    if (resize_sec(conf->secs + conf->sec_num) < 0) {
        ST_WARNING("Failed to resize_sec.");
        goto ERR;
    }

    conf->sec_num++;
    return conf->secs + (conf->sec_num - 1);

ERR:
    return NULL;
}

static FILE *g_cur_fp = NULL;
static FILE *g_global_fp = NULL;

int st_conf_add_param(st_conf_section_t *sec, const char *key,
        const char *value)
{
    st_conf_param_t *param;
    int i;

    param = NULL;
    for (i = 0; i < sec->param_num; i++) {
        if (strcasecmp(sec->param[i].key, key) == 0) {
            param = sec->param + i;
            break;
        }
    }

    if (param == NULL) {
        param = sec->param + sec->param_num;

        sec->param_num++;
        if (resize_sec(sec) < 0) {
            ST_WARNING("Failed to resize_sec.");
            return -1;
        }
    }

    param->used = 0;
    strncpy(param->key, key, MAX_ST_CONF_LEN);
    param->key[MAX_ST_CONF_LEN - 1] = 0;
    strncpy(param->value, value, MAX_ST_CONF_LEN);
    param->value[MAX_ST_CONF_LEN - 1] = 0;

    return 0;
}

int st_resolve_param(const char *line, st_conf_t *pconf,
        st_conf_section_t** sec)
{
    char buffer[MAX_ST_CONF_LINE_LEN];
    char *work = NULL;
    char *sec_conf = NULL;
    int i;
    int j;
    int len;

    if (line == NULL || sec == NULL || *sec == NULL) {
        return -1;
    }
    work = strrchr(line, '\r');
    if (work != NULL) {
        *work = '\0';
    }
    work = strrchr(line, '\n');
    if (work != NULL) {
        *work = '\0';
    }

    len = (int) strlen(line);
    j = 0;

    for (i = 0; i < len; i++) {
        if (line[i] != ' ' && line[i] != '\t') {
            buffer[j] = line[i];
            j++;
        }
        if (j >= MAX_ST_CONF_LINE_LEN) {
            return -1;
        }
    }
    len = j;
    if (len <= 0) {
        return 0;
    }
    buffer[len] = '\0';

    if (buffer[0] == '#') {
        return 0;
    } else if (buffer[0] == '[') {
        if (g_global_fp != g_cur_fp) {
            ST_WARNING("section can not be nested in section.");
            return -1;
        }

        work = strrchr(buffer, ']');
        if (work != NULL) {
            *work = '\0';
        } else {
            ST_WARNING("Not closed '['.");
            return -1;
        }

        if (buffer[1] == '#') {
            work = buffer + 2;
        } else {
            work = buffer + 1;
        }

        sec_conf = strchr(work, ':');
        if (sec_conf != NULL) {
            *sec_conf = 0;
            sec_conf++;

            g_cur_fp = fopen(sec_conf, "rb");
            if (g_cur_fp == NULL) {
                ST_WARNING("Failed to open conf[%s] for section[%s].",
                        sec_conf, work);
                return -1;
            }
        }

        *sec = st_conf_new_sec(pconf, work);
        if ((*sec) == NULL) {
            ST_WARNING("Failed to st_conf_new_sec.");
            return -1;
        }

        if (buffer[1] == '#') {
            (*sec)->comment_out = 1;
        }
        return 0;
    }

    if ((*sec)->comment_out != 0) {
        return 0;
    }

    work = strchr(buffer, ':');
    if (work == NULL) {
        return 0;
    }
    *work = 0;
    work++;

    if (st_conf_add_param(*sec, buffer, work) < 0) {
        ST_WARNING("Failed to st_conf_add_param. key[%s], value[$s]",
                buffer, work);
        return -1;
    }

    return 1;
}

st_conf_section_t* st_conf_def_sec(st_conf_t *conf)
{
    int s;

    for (s = 0; s < conf->sec_num; s++) {
        if (strcasecmp(conf->secs[s].name, DEF_SEC_NAME) == 0) {
            return conf->secs + s;
        }
    }

    return NULL;
}

st_conf_t* st_conf_create()
{
    st_conf_t *pconf = NULL;

    pconf = (st_conf_t *) malloc(sizeof(st_conf_t));
    if (pconf == NULL) {
        ST_WARNING("Failed to malloc st_conf.");
        goto ERR;
    }
    memset(pconf, 0, sizeof(st_conf_t));

    if (st_conf_new_sec(pconf, DEF_SEC_NAME) == NULL) {
        ST_WARNING("Failed to st_conf_new_sec.");
        goto ERR;
    }

    return pconf;

ERR:
    safe_st_conf_destroy(pconf);
    return NULL;
}

int st_conf_load(st_conf_t *st_conf, const char *conf_file)
{
    char line[MAX_ST_CONF_LINE_LEN];
    st_conf_section_t *cur_sec = NULL;
    int ch;

    ST_CHECK_PARAM(st_conf == NULL || conf_file == NULL, -1);

    cur_sec = st_conf_def_sec(st_conf);
    if (cur_sec == NULL) {
        ST_WARNING("Error: No default section.");
        goto ERR;
    }

    g_global_fp = fopen(conf_file, "rb");
    if (g_global_fp == NULL) {
        goto ERR;
    }

    g_cur_fp = g_global_fp;
    while (fgets(line, MAX_ST_CONF_LINE_LEN, g_cur_fp)) {
        if (st_resolve_param(line, st_conf, &cur_sec) < 0) {
            ST_WARNING("Failed to st_resolve_param.");
            goto ERR;
        }

        if (g_cur_fp != g_global_fp) {
            ch = fgetc(g_cur_fp);
            if (feof(g_cur_fp)) {
                fclose(g_cur_fp);
                g_cur_fp = g_global_fp;
            } else {
                ungetc(ch, g_cur_fp);
            }
        }
    }

    fclose(g_global_fp);

    return 0;

ERR:
    safe_fclose(g_global_fp);
    safe_fclose(g_cur_fp);
    return -1;
}

int st_conf_get_str(st_conf_t *pconf, const char *sec_name,
        const char *key, char *value, int vlen, int *sec_i)
{
    char name[MAX_ST_CONF_LEN];
    int s;
    int p;

    if (pconf == NULL || key == NULL || value == NULL || vlen <= 0) {
        if (sec_i != NULL) {
            *sec_i = -1;
        }
        return -1;
    }

    if (sec_name == NULL || sec_name[0] == '\0') {
        strncpy(name, DEF_SEC_NAME, MAX_ST_CONF_LEN);
    } else {
        strncpy(name, sec_name, MAX_ST_CONF_LEN);
    }
    name[MAX_ST_CONF_LEN - 1] = '\0';

    for (s = 0; s < pconf->sec_num; s++) {
        if (strcasecmp(pconf->secs[s].name, name) == 0) {
            for (p = 0; p < pconf->secs[s].param_num; p++) {
                if (strcasecmp(pconf->secs[s].param[p].key, key) == 0) {
                    pconf->secs[s].param[p].used = 1;
                    strncpy(value, pconf->secs[s].param[p].value, vlen);
                    value[vlen - 1] = 0;
                    break;
                }
            }

            if (p >= pconf->secs[s].param_num) {
                if (sec_i != NULL) {
                    *sec_i = s;
                }
                return -1;
            }

            break;
        }
    }

    if (s >= pconf->sec_num) {
        if (sec_i != NULL) {
            *sec_i = -1;
        }
        return -1;
    }

    if (sec_i != NULL) {
        *sec_i = s;
    }
    return 0;
}

int st_conf_get_str_def(st_conf_t *pconf, const char *sec_name,
        const char *key, char *value, int vlen, const char *default_value)
{
    st_conf_section_t *sec;
    int sec_i = -1;

    if (st_conf_get_str(pconf, sec_name, key, value, vlen, &sec_i) < 0) {
        strncpy(value, default_value, vlen);
        value[vlen - 1] = 0;

        if (sec_i < 0) {
            sec = st_conf_new_sec(pconf, sec_name);
            if (sec == NULL) {
                ST_WARNING("Failed to st_conf_new_sec.");
                return -1;
            }
        } else {
            sec = pconf->secs + sec_i;
        }

        strncpy(sec->def_param[sec->def_param_num].key, key,
                MAX_ST_CONF_LEN);
        sec->def_param[sec->def_param_num].key[MAX_ST_CONF_LEN - 1] = 0;
        strncpy(sec->def_param[sec->def_param_num].value, default_value,
                MAX_ST_CONF_LEN);
        sec->def_param[sec->def_param_num].value[MAX_ST_CONF_LEN - 1] = 0;
        sec->def_param_num++;

        if (resize_sec(sec) < 0) {
            ST_WARNING("Failed to resize_sec.");
            return -1;
        }
    }

    return 0;
}

int st_conf_get_bool(st_conf_t *pconf, const char *sec_name,
        const char *key, bool *value, int *sec_i)
{
    char v[MAX_ST_CONF_LINE_LEN];

    if (st_conf_get_str(pconf, sec_name, key, v,
                MAX_ST_CONF_LINE_LEN, sec_i) < 0) {
        return -1;
    }

    if (v[0] == '\0' // for no argument options, e.g. --help
            || strncmp(v, "1", 2) == 0
            || strncasecmp(v, "T", 2) == 0
            || strncasecmp(v, "TRUE", 5) == 0) {
        (*value) = true;
    } else if (strncmp(v, "0", 2) == 0
            || strncasecmp(v, "F", 2) == 0
            || strncasecmp(v, "FALSE", 6) == 0) {
        (*value) = false;
    } else {
        ST_WARNING("Unkown bool value[%s], should be \"True\" or \"False\".",
                v);
        return -1;
    }

    return 0;
}

int st_conf_get_bool_def(st_conf_t *pconf, const char *sec_name,
        const char *key, bool *value, bool default_value)
{
    st_conf_section_t *sec;
    int sec_i = -1;

    if (st_conf_get_bool(pconf, sec_name, key, value, &sec_i) < 0) {
        *value = default_value;

        if (sec_i < 0) {
            sec = st_conf_new_sec(pconf, sec_name);
            if (sec == NULL) {
                ST_WARNING("Failed to st_conf_new_sec.");
                return -1;
            }
        } else {
            sec = pconf->secs + sec_i;
        }

        strncpy(sec->def_param[sec->def_param_num].key, key,
                MAX_ST_CONF_LEN);
        sec->def_param[sec->def_param_num].key[MAX_ST_CONF_LEN - 1] = 0;
        if (default_value) {
            snprintf(sec->def_param[sec->def_param_num].value,
                    MAX_ST_CONF_LEN, "%s", "True");
        } else {
            snprintf(sec->def_param[sec->def_param_num].value,
                    MAX_ST_CONF_LEN, "%s", "False");
        }
        sec->def_param[sec->def_param_num].value[MAX_ST_CONF_LEN - 1] = 0;
        sec->def_param_num++;

        if (resize_sec(sec) < 0) {
            ST_WARNING("Failed to resize_sec.");
            return -1;
        }
    }
    return 0;
}

int st_conf_get_int(st_conf_t *pconf, const char *sec_name,
        const char *key, int *value, int *sec_i)
{
    char v[MAX_ST_CONF_LINE_LEN];

    if (st_conf_get_str(pconf, sec_name, key, v,
                MAX_ST_CONF_LINE_LEN, sec_i) < 0) {
        return -1;
    }

    if (v[0] == '\0') {
        if (sec_name == NULL || sec_name[0] == '\0') {
            ST_WARNING("Int option[%s] should have arguement", key);
        } else {
            ST_WARNING("Int option[%s^%s] should have arguement",
                    sec_name, key);
        }

        return -1;
    }

    (*value) = atoi(v);
    return 0;
}

int st_conf_get_int_def(st_conf_t *pconf, const char *sec_name,
        const char *key, int *value, int default_value)
{
    st_conf_section_t *sec;
    int sec_i = -1;

    if (st_conf_get_int(pconf, sec_name, key, value, &sec_i) < 0) {
        *value = default_value;

        if (sec_i < 0) {
            sec = st_conf_new_sec(pconf, sec_name);
            if (sec == NULL) {
                ST_WARNING("Failed to st_conf_new_sec.");
                return -1;
            }
        } else {
            sec = pconf->secs + sec_i;
        }

        strncpy(sec->def_param[sec->def_param_num].key, key,
                MAX_ST_CONF_LEN);
        sec->def_param[sec->def_param_num].key[MAX_ST_CONF_LEN - 1] = 0;
        snprintf(sec->def_param[sec->def_param_num].value,
                 MAX_ST_CONF_LEN, "%d", default_value);
        sec->def_param[sec->def_param_num].value[MAX_ST_CONF_LEN - 1] = 0;
        sec->def_param_num++;

        if (resize_sec(sec) < 0) {
            ST_WARNING("Failed to resize_sec.");
            return -1;
        }
    }
    return 0;
}

int st_conf_get_uint(st_conf_t *pconf, const char *sec_name,
        const char *key, unsigned int *value, int *sec_i)
{
    char v[MAX_ST_CONF_LINE_LEN];

    if (st_conf_get_str(pconf, sec_name, key, v,
                MAX_ST_CONF_LINE_LEN, sec_i) < 0) {
        return -1;
    }

    if (v[0] == '\0') {
        if (sec_name == NULL || sec_name[0] == '\0') {
            ST_WARNING("Uint option[%s] should have arguement", key);
        } else {
            ST_WARNING("Uint option[%s^%s] should have arguement",
                    sec_name, key);
        }

        return -1;
    }

    sscanf(v, "%u", value);
    return 0;
}

int st_conf_get_uint_def(st_conf_t *pconf, const char *sec_name,
        const char *key, unsigned int *value, unsigned int default_value)
{
    st_conf_section_t *sec;
    int sec_i = -1;

    if (st_conf_get_uint(pconf, sec_name, key, value, &sec_i) < 0) {
        *value = default_value;

        if (sec_i < 0) {
            sec = st_conf_new_sec(pconf, sec_name);
            if (sec == NULL) {
                ST_WARNING("Failed to st_conf_new_sec.");
                return -1;
            }
        } else {
            sec = pconf->secs + sec_i;
        }
        strncpy(sec->def_param[sec->def_param_num].key, key,
                MAX_ST_CONF_LEN);
        sec->def_param[sec->def_param_num].key[MAX_ST_CONF_LEN - 1] = 0;
        snprintf(sec->def_param[sec->def_param_num].value,
                 MAX_ST_CONF_LEN, "%u", default_value);
        sec->def_param[sec->def_param_num].value[MAX_ST_CONF_LEN - 1] = 0;
        sec->def_param_num++;

        if (resize_sec(sec) < 0) {
            ST_WARNING("Failed to resize_sec.");
            return -1;
        }
    }
    return 0;
}

int st_conf_get_long(st_conf_t *pconf, const char *sec_name,
        const char *key, long *value, int *sec_i)
{
    char v[MAX_ST_CONF_LINE_LEN];

    if (st_conf_get_str(pconf, sec_name, key, v,
                MAX_ST_CONF_LINE_LEN, sec_i) < 0) {
        return -1;
    }

    if (v[0] == '\0') {
        if (sec_name == NULL || sec_name[0] == '\0') {
            ST_WARNING("Int option[%s] should have arguement", key);
        } else {
            ST_WARNING("Int option[%s^%s] should have arguement",
                    sec_name, key);
        }

        return -1;
    }

    (*value) = atoi(v);
    return 0;
}

int st_conf_get_long_def(st_conf_t *pconf, const char *sec_name,
        const char *key, long *value, long default_value)
{
    st_conf_section_t *sec;
    int sec_i = -1;

    if (st_conf_get_long(pconf, sec_name, key, value, &sec_i) < 0) {
        *value = default_value;

        if (sec_i < 0) {
            sec = st_conf_new_sec(pconf, sec_name);
            if (sec == NULL) {
                ST_WARNING("Failed to st_conf_new_sec.");
                return -1;
            }
        } else {
            sec = pconf->secs + sec_i;
        }

        strncpy(sec->def_param[sec->def_param_num].key, key,
                MAX_ST_CONF_LEN);
        sec->def_param[sec->def_param_num].key[MAX_ST_CONF_LEN - 1] = 0;
        snprintf(sec->def_param[sec->def_param_num].value,
                 MAX_ST_CONF_LEN, "%ld", default_value);
        sec->def_param[sec->def_param_num].value[MAX_ST_CONF_LEN - 1] = 0;
        sec->def_param_num++;

        if (resize_sec(sec) < 0) {
            ST_WARNING("Failed to resize_sec.");
            return -1;
        }
    }
    return 0;
}

int st_conf_get_ulong(st_conf_t *pconf, const char *sec_name,
        const char *key, unsigned long *value, int *sec_i)
{
    char v[MAX_ST_CONF_LINE_LEN];

    if (st_conf_get_str(pconf, sec_name, key, v,
                MAX_ST_CONF_LINE_LEN, sec_i) < 0) {
        return -1;
    }

    if (v[0] == '\0') {
        if (sec_name == NULL || sec_name[0] == '\0') {
            ST_WARNING("Ulong option[%s] should have arguement", key);
        } else {
            ST_WARNING("Ulong option[%s^%s] should have arguement",
                    sec_name, key);
        }

        return -1;
    }

    sscanf(v, "%lu", value);
    return 0;
}

int st_conf_get_ulong_def(st_conf_t *pconf, const char *sec_name,
        const char *key, unsigned long *value, unsigned long default_value)
{
    st_conf_section_t *sec;
    int sec_i = -1;

    if (st_conf_get_ulong(pconf, sec_name, key, value, &sec_i) < 0) {
        *value = default_value;

        if (sec_i < 0) {
            sec = st_conf_new_sec(pconf, sec_name);
            if (sec == NULL) {
                ST_WARNING("Failed to st_conf_new_sec.");
                return -1;
            }
        } else {
            sec = pconf->secs + sec_i;
        }
        strncpy(sec->def_param[sec->def_param_num].key, key,
                MAX_ST_CONF_LEN);
        sec->def_param[sec->def_param_num].key[MAX_ST_CONF_LEN - 1] = 0;
        snprintf(sec->def_param[sec->def_param_num].value,
                 MAX_ST_CONF_LEN, "%lu", default_value);
        sec->def_param[sec->def_param_num].value[MAX_ST_CONF_LEN - 1] = 0;
        sec->def_param_num++;

        if (resize_sec(sec) < 0) {
            ST_WARNING("Failed to resize_sec.");
            return -1;
        }
    }
    return 0;
}

int st_conf_get_double(st_conf_t *pconf, const char *sec_name,
        const char *key, double *value, int *sec_i)
{
    char v[MAX_ST_CONF_LINE_LEN];

    if (st_conf_get_str(pconf, sec_name, key, v,
                MAX_ST_CONF_LINE_LEN, sec_i) < 0) {
        return -1;
    }

    if (v[0] == '\0') {
        if (sec_name == NULL || sec_name[0] == '\0') {
            ST_WARNING("Float option[%s] should have arguement", key);
        } else {
            ST_WARNING("Float option[%s^%s] should have arguement",
                    sec_name, key);
        }

        return -1;
    }

    (*value) = atof(v);
    return 0;
}

int st_conf_get_double_def(st_conf_t *pconf, const char *sec_name,
        const char *key, double *value, double default_value)
{
    st_conf_section_t *sec;
    int sec_i = -1;

    if (st_conf_get_double(pconf, sec_name, key, value, &sec_i) < 0) {
        *value = default_value;

        if (sec_i < 0) {
            sec = st_conf_new_sec(pconf, sec_name);
            if (sec == NULL) {
                ST_WARNING("Failed to st_conf_new_sec.");
                return -1;
            }
        } else {
            sec = pconf->secs + sec_i;
        }
        strncpy(sec->def_param[sec->def_param_num].key, key,
                MAX_ST_CONF_LEN);
        sec->def_param[sec->def_param_num].key[MAX_ST_CONF_LEN - 1] = 0;
        snprintf(sec->def_param[sec->def_param_num].value,
                 MAX_ST_CONF_LEN, "%g", default_value);
        sec->def_param[sec->def_param_num].value[MAX_ST_CONF_LEN - 1] = 0;
        sec->def_param_num++;
        if (resize_sec(sec) < 0) {
            ST_WARNING("Failed to resize_sec.");
            return -1;
        }
    }
    return 0;
}

void st_conf_destroy(st_conf_t * pconf)
{
    int i;
    if (pconf != NULL) {
        for (i = 0; i < pconf->sec_num; i++) {
            if (pconf->secs[i].param != NULL) {
                free(pconf->secs[i].param);
                pconf->secs[i].param = NULL;
            }
            pconf->secs[i].param_cap = 0;
            pconf->secs[i].param_num = 0;

            if (pconf->secs[i].def_param != NULL) {
                free(pconf->secs[i].def_param);
                pconf->secs[i].def_param = NULL;
            }
            pconf->secs[i].def_param_cap = 0;
            pconf->secs[i].def_param_num = 0;
        }
        if (pconf->secs != NULL) {
            free(pconf->secs);
            pconf->secs = NULL;
        }
        pconf->sec_cap = 0;
        pconf->sec_num = 0;
    }
}

static char* st_conf_normalize_key(char *key)
{
    char *p;

    p = key;
    while (*p) {
        if (*p == '-') {
            *p = '_';
        } else {
            *p = toupper(*p);
        }
        p++;
    }

    return key;
}

void st_conf_show(st_conf_t *pconf, const char *header)
{
    int s;
    int p;

    if (pconf == NULL) {
        return;
    }
    if (header != NULL) {
        ST_CLEAN("%s", header);
    }
    for (s = 0; s < pconf->sec_num; s++) {
        if (pconf->secs[s].comment_out != 0) {
            continue;
        }
        ST_CLEAN("[%s]", st_conf_normalize_key(pconf->secs[s].name));
        for (p = 0; p < pconf->secs[s].param_num; p++) {
            ST_CLEAN("%s{%s : %s}", pconf->secs[s].param[p].used ? "" : "*",
                    st_conf_normalize_key(pconf->secs[s].param[p].key),
                    pconf->secs[s].param[p].value);
        }
        for (p = 0; p < pconf->secs[s].def_param_num; p++) {
            ST_CLEAN("{%s : %s}#",
                    st_conf_normalize_key(pconf->secs[s].def_param[p].key),
                    pconf->secs[s].def_param[p].value);
        }
        ST_CLEAN("");
    }
}
