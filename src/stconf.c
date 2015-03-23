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

#include "stlog.h"
#include "stconf.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SEC_NUM     10
#define PARAM_NUM    100

static int resize_sec(stconf_section_t *sec)
{
    if (sec->param_num >= sec->param_cap) {
        sec->param_cap += PARAM_NUM;
        sec->param = (stconf_param_t *) realloc(sec->param, 
                    sec->param_cap * sizeof(stconf_param_t));
        if (sec->param == NULL) {
            ST_WARNING("Failed to realloc param for sec.");
            goto ERR;
        }
        memset(sec->param + sec->param_num, 0, 
                PARAM_NUM * sizeof(stconf_param_t));
    }

    if (sec->def_param_num >= sec->def_param_cap) {
        sec->def_param_cap += PARAM_NUM;
        sec->def_param = (stconf_param_t *) realloc(sec->def_param, 
                    sec->def_param_cap * sizeof(stconf_param_t));
        if (sec->def_param == NULL) {
            ST_WARNING("Failed to realloc def_param for sec.");
            goto ERR;
        }
        memset(sec->def_param + sec->def_param_num, 0, 
                PARAM_NUM * sizeof(stconf_param_t));
    }

    return 0;
ERR:
    return -1;
}

static stconf_section_t* new_sec(stconf_t *conf, const char *name)
{
    if (conf->sec_num >= conf->sec_cap) {
        conf->sec_cap += SEC_NUM;
        conf->secs = (stconf_section_t *) realloc(conf->secs, 
                    conf->sec_cap * sizeof(stconf_section_t));
        if (conf->secs == NULL) {
            ST_WARNING("Failed to realloc secs.");
            goto ERR;
        }
        memset(conf->secs + conf->sec_num, 0, 
                SEC_NUM * sizeof(stconf_section_t));
    }

    strncpy(conf->secs[conf->sec_num].name, name, MAX_STCONF_LEN);
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

int resolve_param(const char *line, stconf_t *pconf, 
        stconf_section_t** sec)
{
    char buffer[MAX_STCONF_LINE_LEN];
    stconf_param_t *param;
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
        if (j >= MAX_STCONF_LINE_LEN) {
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

        *sec = new_sec(pconf, work);
        if ((*sec) == NULL) {
            ST_WARNING("Failed to new_sec.");
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

    param = NULL;
    for (i = 0; i < (*sec)->param_num; i++) {
        if (strcasecmp((*sec)->param[i].key, buffer) == 0) {
            param = (*sec)->param + i;
            break;
        }
    }

    if (param == NULL) {
        param = (*sec)->param + (*sec)->param_num;

        (*sec)->param_num++;
        if (resize_sec(*sec) < 0) {
            ST_WARNING("Failed to resize_sec.");
            return -1;
        }
    }

    param->used = 0;
    strncpy(param->key, buffer, MAX_STCONF_LEN);
    param->key[MAX_STCONF_LEN - 1] = 0;
    strncpy(param->value, work, MAX_STCONF_LEN);
    param->value[MAX_STCONF_LEN - 1] = 0;

    return 1;
}

stconf_t *init_stconf(const char *conf_file)
{
    char line[MAX_STCONF_LINE_LEN];
    stconf_t *pconf = NULL;
    stconf_section_t *cur_sec = NULL;
    int ch;

    if (conf_file == NULL) {
        return NULL;
    }

    pconf = (stconf_t *) malloc(sizeof(stconf_t));
    if (pconf == NULL) {
        goto ERR;
    }
    memset(pconf, 0, sizeof(stconf_t));

    cur_sec = new_sec(pconf, "__def_sec__");
    if (cur_sec == NULL) {
        ST_WARNING("Failed to new_sec.");
        goto ERR;
    }

    g_global_fp = fopen(conf_file, "rb");
    if (g_global_fp == NULL) {
        goto ERR;
    }

    g_cur_fp = g_global_fp;
    while (fgets(line, MAX_STCONF_LINE_LEN, g_cur_fp)) {
        if (resolve_param(line, pconf, &cur_sec) < 0) {
            ST_WARNING("Failed to resolve_param.");
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

    return pconf;

ERR:
    if (g_global_fp != NULL) {
        fclose(g_global_fp);
        g_global_fp = NULL;
    }
    if (g_cur_fp != NULL) {
        fclose(g_cur_fp);
        g_cur_fp = NULL;
    }
    free_stconf(pconf);
    return NULL;
}

int getstrconf(stconf_t * pconf, const char *sec_name,
        const char *key, char *value, int vlen, int *sec_i)
{
    int s;
    int p;

    if (pconf == NULL || key == NULL || value == NULL || vlen <= 0) {
        if (sec_i != NULL) {
            *sec_i = -1;
        }
        return -1;
    }

    if (sec_name == NULL || sec_name[0] == '\0') {
        s = 0;
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

        if (sec_i != NULL) {
            *sec_i = s;
        }
        return 0;
    }

    for (s = 0; s < pconf->sec_num; s++) {
        if (strcasecmp(pconf->secs[s].name, sec_name) == 0) {
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

int getstrconfdef(stconf_t * pconf, const char *sec_name,
        const char *key, char *value, int vlen, const char *default_value)
{
    stconf_section_t *sec;
    int sec_i = -1;

    if (getstrconf(pconf, sec_name, key, value, vlen, &sec_i) < 0) {
        strncpy(value, default_value, vlen);
        value[vlen - 1] = 0;

        if (sec_i < 0) {
            sec = new_sec(pconf, sec_name);
            if (sec == NULL) {
                ST_WARNING("Failed to new_sec.");
                return -1;
            }
        } else {
            sec = pconf->secs + sec_i;
        }

        strncpy(sec->def_param[sec->def_param_num].key, key,
                MAX_STCONF_LEN);
        sec->def_param[sec->def_param_num].key[MAX_STCONF_LEN - 1] = 0;
        strncpy(sec->def_param[sec->def_param_num].value, default_value,
                MAX_STCONF_LEN);
        sec->def_param[sec->def_param_num].value[MAX_STCONF_LEN - 1] = 0;
        sec->def_param_num++;

        if (resize_sec(sec) < 0) {
            ST_WARNING("Failed to resize_sec.");
            return -1;
        }
    }

    return 0;
}

int getboolconf(stconf_t * pconf, const char *sec_name,
        const char *key, bool *value, int *sec_i)
{
    char v[MAX_STCONF_LINE_LEN];

    if (getstrconf(pconf, sec_name, key, v, 
                MAX_STCONF_LINE_LEN, sec_i) < 0) {
        return -1;
    }

    if (strncmp(v, "1", 2) == 0
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

int getboolconfdef(stconf_t * pconf, const char *sec_name,
        const char *key, bool *value, bool default_value)
{
    stconf_section_t *sec;
    int sec_i = -1;

    if (getboolconf(pconf, sec_name, key, value, &sec_i) < 0) {
        *value = default_value;

        if (sec_i < 0) {
            sec = new_sec(pconf, sec_name);
            if (sec == NULL) {
                ST_WARNING("Failed to new_sec.");
                return -1;
            }
        } else {
            sec = pconf->secs + sec_i;
        }

        strncpy(sec->def_param[sec->def_param_num].key, key,
                MAX_STCONF_LEN);
        sec->def_param[sec->def_param_num].key[MAX_STCONF_LEN - 1] = 0;
        if (default_value) {
            snprintf(sec->def_param[sec->def_param_num].value,
                    MAX_STCONF_LEN, "%s", "True");
        } else {
            snprintf(sec->def_param[sec->def_param_num].value,
                    MAX_STCONF_LEN, "%s", "False");
        }
        sec->def_param[sec->def_param_num].value[MAX_STCONF_LEN - 1] = 0;
        sec->def_param_num++;

        if (resize_sec(sec) < 0) {
            ST_WARNING("Failed to resize_sec.");
            return -1;
        }
    }
    return 0;
}

int getintconf(stconf_t * pconf, const char *sec_name,
        const char *key, int *value, int *sec_i)
{
    char v[MAX_STCONF_LINE_LEN];

    if (getstrconf(pconf, sec_name, key, v, 
                MAX_STCONF_LINE_LEN, sec_i) < 0) {
        return -1;
    }
    (*value) = atoi(v);
    return 0;
}

int getintconfdef(stconf_t * pconf, const char *sec_name,
        const char *key, int *value, int default_value)
{
    stconf_section_t *sec;
    int sec_i = -1;

    if (getintconf(pconf, sec_name, key, value, &sec_i) < 0) {
        *value = default_value;

        if (sec_i < 0) {
            sec = new_sec(pconf, sec_name);
            if (sec == NULL) {
                ST_WARNING("Failed to new_sec.");
                return -1;
            }
        } else {
            sec = pconf->secs + sec_i;
        }

        strncpy(sec->def_param[sec->def_param_num].key, key,
                MAX_STCONF_LEN);
        sec->def_param[sec->def_param_num].key[MAX_STCONF_LEN - 1] = 0;
        snprintf(sec->def_param[sec->def_param_num].value,
                 MAX_STCONF_LEN, "%d", default_value);
        sec->def_param[sec->def_param_num].value[MAX_STCONF_LEN - 1] = 0;
        sec->def_param_num++;

        if (resize_sec(sec) < 0) {
            ST_WARNING("Failed to resize_sec.");
            return -1;
        }
    }
    return 0;
}

int getuintconf(stconf_t * pconf, const char *sec_name,
        const char *key, unsigned int *value, int *sec_i)
{
    char v[MAX_STCONF_LINE_LEN];

    if (getstrconf(pconf, sec_name, key, v, 
                MAX_STCONF_LINE_LEN, sec_i) < 0) {
        return -1;
    }
    sscanf(v, "%u", value);
    return 0;
}

int getuintconfdef(stconf_t * pconf, const char *sec_name,
        const char *key, unsigned int *value, unsigned int default_value)
{
    stconf_section_t *sec;
    int sec_i = -1;

    if (getuintconf(pconf, sec_name, key, value, &sec_i) < 0) {
        *value = default_value;

        if (sec_i < 0) {
            sec = new_sec(pconf, sec_name);
            if (sec == NULL) {
                ST_WARNING("Failed to new_sec.");
                return -1;
            }
        } else {
            sec = pconf->secs + sec_i;
        }
        strncpy(sec->def_param[sec->def_param_num].key, key,
                MAX_STCONF_LEN);
        sec->def_param[sec->def_param_num].key[MAX_STCONF_LEN - 1] = 0;
        snprintf(sec->def_param[sec->def_param_num].value,
                 MAX_STCONF_LEN, "%u", default_value);
        sec->def_param[sec->def_param_num].value[MAX_STCONF_LEN - 1] = 0;
        sec->def_param_num++;

        if (resize_sec(sec) < 0) {
            ST_WARNING("Failed to resize_sec.");
            return -1;
        }
    }
    return 0;
}

int getfloatconf(stconf_t * pconf, const char *sec_name,
        const char *key, float *value, int *sec_i)
{
    char v[MAX_STCONF_LINE_LEN];

    if (getstrconf(pconf, sec_name, key, v, 
                MAX_STCONF_LINE_LEN, sec_i) < 0) {
        return -1;
    }
    (*value) = atof(v);
    return 0;
}

int getfloatconfdef(stconf_t * pconf, const char *sec_name,
        const char *key, float *value, float default_value)
{
    stconf_section_t *sec;
    int sec_i = -1;

    if (getfloatconf(pconf, sec_name, key, value, &sec_i) < 0) {
        *value = default_value;

        if (sec_i < 0) {
            sec = new_sec(pconf, sec_name);
            if (sec == NULL) {
                ST_WARNING("Failed to new_sec.");
                return -1;
            }
        } else {
            sec = pconf->secs + sec_i;
        }
        strncpy(sec->def_param[sec->def_param_num].key, key,
                MAX_STCONF_LEN);
        sec->def_param[sec->def_param_num].key[MAX_STCONF_LEN - 1] = 0;
        snprintf(sec->def_param[sec->def_param_num].value,
                 MAX_STCONF_LEN, "%f", default_value);
        sec->def_param[sec->def_param_num].value[MAX_STCONF_LEN - 1] = 0;
        sec->def_param_num++;
        if (resize_sec(sec) < 0) {
            ST_WARNING("Failed to resize_sec.");
            return -1;
        }
    }
    return 0;
}

void free_stconf(stconf_t * pconf)
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
        free(pconf);
    }
}

void show_stconf(stconf_t * pconf, const char *header)
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
        ST_CLEAN("[%s]", pconf->secs[s].name);
        for (p = 0; p < pconf->secs[s].param_num; p++) {
            ST_CLEAN("%s{%s : %s}", pconf->secs[s].param[p].used ? "" : "*",
                    pconf->secs[s].param[p].key, 
                    pconf->secs[s].param[p].value);
        }
        for (p = 0; p < pconf->secs[s].def_param_num; p++) {
            ST_CLEAN("{%s : %s}#", pconf->secs[s].def_param[p].key,
                    pconf->secs[s].def_param[p].value);
        }
    }
}

