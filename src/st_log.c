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
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>
#include <sys/stat.h>

#include "st_io.h"
#include "st_log.h"

static FILE *g_normal_fp = NULL;
static FILE *g_wf_fp = NULL;
static int g_mask = 0xff;

static int g_mt = 0;
static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;

static char *st_time(char *t_ime)
{
    time_t tt;
    struct tm vtm;

    time(&tt);
    localtime_r(&tt, &vtm);
    vtm.tm_year += 1900;
    sprintf(t_ime, "%02d-%02d %02d:%02d:%02d", vtm.tm_mon+1, 
            vtm.tm_mday, vtm.tm_hour, vtm.tm_min, vtm.tm_sec);

    return  t_ime;
}

static int st_log_write_ex(FILE *fp, const char *fmt, va_list args)
{
    char now[20];

    st_time(now);
    fprintf(fp, "%s: ", now);

    vfprintf(fp, fmt, args);
    fprintf(fp, "\n");

    return 0;
}

#define MAX_FILENAME_LEN 2048
static FILE *st_open_file(const char *name, const char *mode)
{
    int path_len;
    FILE *fp;
    char *path_end;
    char path[MAX_FILENAME_LEN];

    fp = st_fopen(name, mode);
    if (fp != NULL)
        return fp;

    path_end = strrchr(name, '/');
    if (path_end != NULL)
        path_len =
            path_end - name >
            MAX_FILENAME_LEN ? MAX_FILENAME_LEN : path_end - name;
    else
        path_len =
            strlen(name) >
            MAX_FILENAME_LEN ? MAX_FILENAME_LEN : strlen(name);
    strncpy(path, name, path_len);
    path[path_len] = '\0';

    mkdir(path, 0700);

    return st_fopen(name, mode);
}

int st_log_load_opt(st_log_opt_t *log_opt, st_opt_t *st_opt,
        const char *sec_name)
{
    ST_CHECK_PARAM(log_opt == NULL || st_opt == NULL, -1);

    ST_OPT_GET_STR(st_opt, "LOG_FILE",
            log_opt->file, MAX_DIR_LEN, DEFAULT_LOGFILE, "Log file");
    ST_OPT_GET_INT(st_opt, "LOG_LEVEL", log_opt->level,
                     DEFAULT_LOGLEVEL, "Log level (1-8)");

    return 0;

ST_OPT_ERR:
    return -1;
}

int st_log_open(st_log_opt_t *log_opt)
{
    char wf_file[2048];
    char now[20];

    if (log_opt == NULL || log_opt->file[0] == '\0'
            || (log_opt->file[0] == '-' && log_opt->file[1] == '\0')
            || (strcmp(log_opt->file, "/dev/stdout") == 0)) {
        g_normal_fp = stdout;
        g_wf_fp = stderr;
    } else if (log_opt != NULL 
            && strcmp(log_opt->file, "/dev/stderr") == 0) {
        g_normal_fp = stderr;
        g_wf_fp = stderr;
    } else {
        g_normal_fp = st_open_file(log_opt->file, "a");
        if (g_normal_fp == NULL) {
            fprintf(stderr, "Failed to open log file[%s]\n", log_opt->file);
            g_normal_fp = stdout;
        }

        snprintf(wf_file, 2048, "%s.wf", log_opt->file);
        g_wf_fp = st_open_file(wf_file, "a");
        if (g_wf_fp == NULL) {
            fprintf(stderr, "Failed to open wf log file[%s]\n", wf_file);
            g_wf_fp = stderr;
        }
    }

    st_time(now);
    fprintf(g_normal_fp, "%s: ========= OPEN LOG =========\n", now);
    fprintf(g_wf_fp, "%s: ========= OPEN LOG WF =========\n", now);

    fflush(g_normal_fp);
    fflush(g_wf_fp);

    g_mask = (log_opt == NULL) ? DEFAULT_LOGLEVEL : log_opt->level;

    return 0;
}

int st_log_open_mt(st_log_opt_t *log_opt)
{
    g_mt = 1;

    return st_log_open(log_opt);
}

int st_log_write(int lev, const char* fmt, ...) 
{
    va_list args;
    FILE *fp;
    pthread_t tid;
    size_t i;
    int ret;

    if (g_normal_fp == NULL) {
        g_normal_fp = stdout; 
    }

    if (g_wf_fp == NULL) {
        g_wf_fp = stderr;
    }

    if (lev > g_mask) {
        return 0;
    }

    if (g_mt) {
        (void)pthread_mutex_lock(&g_lock);
    }
    va_start(args, fmt);
    switch(lev) {
        case ST_LOG_LEV_CLEANEST:
            vfprintf(g_normal_fp, fmt, args);
            va_end(args);
            fflush(g_normal_fp);
            if (g_mt) {
                (void)pthread_mutex_unlock(&g_lock);
            }
            return 0;
        case ST_LOG_LEV_CLEANER:
            vfprintf(g_normal_fp, fmt, args);
            fprintf(g_normal_fp, "\n");
            va_end(args);
            fflush(g_normal_fp);
            if (g_mt) {
                (void)pthread_mutex_unlock(&g_lock);
            }
            return 0;
        case ST_LOG_LEV_CLEAN:
            fp = g_normal_fp;
            break;
        case ST_LOG_LEV_FATAL:
            fp = g_wf_fp;
            fprintf(fp, "FATAL: ");
            break;
        case ST_LOG_LEV_WARNING:
            fp = g_wf_fp;
            fprintf(fp, "WARNING: ");
            break;
        case ST_LOG_LEV_NOTICE:
            fp = g_normal_fp;
            fprintf(fp, "NOTICE: ");
            break;
        case ST_LOG_LEV_TRACE:
            fp = g_normal_fp;
            fprintf(fp, "TRACE: ");
            break;
        case ST_LOG_LEV_DEBUG:
            fp = g_normal_fp;
            fprintf(fp, "DEBUG: ");
            break;
        default:
            va_end(args);
            if (g_mt) {
                (void)pthread_mutex_unlock(&g_lock);
            }
            return 0;
    }

    if (g_mt) {
        tid = pthread_self();
        fprintf(fp, "-- ");
        for (i = sizeof(tid); i; --i) {
            fprintf(fp, "%02x", *(((unsigned char*) &tid) + i - 1));
        }
        fprintf(fp, " -- ");
    }

    ret = st_log_write_ex(fp, fmt, args);
    va_end(args);

    fflush(fp);

    if (g_mt) {
        (void)pthread_mutex_unlock(&g_lock);
    }

    return ret;
}

int st_log_close(int iserr) 
{
    char now[20];

    st_time(now);

    if(iserr) {
        fprintf(g_normal_fp, "%s: " 
                "========= < ! > Abnormally End =========\n", now);
        fprintf(g_wf_fp, "%s: " 
                "========= < ! > Abnormally End =========\n", now);
    } else {
        fprintf(g_normal_fp, "%s: " 
                "========= < - > Normally End =========\n", now);
        fprintf(g_wf_fp, "%s: " 
                "========= < - > Normally End =========\n", now);
    }

    fflush(g_normal_fp);
    fflush(g_wf_fp);

    if (g_normal_fp != NULL && g_normal_fp != stdout
            && g_normal_fp != stderr) {
        fclose(g_normal_fp);
        g_normal_fp = NULL;
    }

    if (g_wf_fp != NULL && g_wf_fp != stdout && g_wf_fp != stderr) {
        fclose(g_wf_fp);
        g_wf_fp = NULL;
    }
    return 0;
}
