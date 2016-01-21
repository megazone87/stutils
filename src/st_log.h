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

#ifndef _ST_LOG_H_
#define _ST_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stutils/st_macro.h>
#include "st_opt.h"

#define ST_LOG_LEV_CLEANEST     0x01
#define ST_LOG_LEV_CLEANER      0x02
#define ST_LOG_LEV_CLEAN        0x03
#define ST_LOG_LEV_FATAL	    0x04
#define ST_LOG_LEV_WARNING	    0x05
#define ST_LOG_LEV_NOTICE       0x06
#define ST_LOG_LEV_TRACE	    0x07
#define ST_LOG_LEV_DEBUG	    0x08

typedef struct _st_log_opt_t_ {
    char file[MAX_DIR_LEN];
    int  level;
} st_log_opt_t;
 
#define DEFAULT_LOGFILE         "/dev/stderr"
#define DEFAULT_LOGLEVEL        8

int st_log_load_opt(st_log_opt_t *log_opt, st_opt_t *st_opt,
        const char *sec_name);

int st_log_open(st_log_opt_t *log_opt);

int st_log_open_mt(st_log_opt_t *log_opt);

int st_log_write(const int lev, const char* fmt, ... );

int st_log_close(int err);

/*@ignore@*/ 
#define ST_LOG(lev, fmt, ...) \
    st_log_write(lev, "[%s:%d<<%s>>] " fmt, __FILE__, __LINE__, __func__, \
    ##__VA_ARGS__);

#define ST_FATAL(fmt, ...) \
    ST_LOG(ST_LOG_LEV_FATAL, fmt, ##__VA_ARGS__);
    
#define ST_WARNING(fmt, ...) \
    ST_LOG(ST_LOG_LEV_WARNING, fmt, ##__VA_ARGS__);
    
#define ST_NOTICE(fmt, ...) \
    ST_LOG(ST_LOG_LEV_NOTICE, fmt, ##__VA_ARGS__);
    
#define ST_TRACE(fmt, ...) \
    ST_LOG(ST_LOG_LEV_TRACE, fmt, ##__VA_ARGS__);
    
#define ST_DEBUG(fmt, ...) \
    ST_LOG(ST_LOG_LEV_DEBUG, fmt, ##__VA_ARGS__);

#define ST_CLEANEST(fmt, ...) \
    st_log_write(ST_LOG_LEV_CLEANEST, fmt, ##__VA_ARGS__);

#define ST_CLEANER(fmt, ...) \
    st_log_write(ST_LOG_LEV_CLEANER, fmt, ##__VA_ARGS__);

#define ST_CLEAN(fmt, ...) \
    st_log_write(ST_LOG_LEV_CLEAN, fmt, ##__VA_ARGS__);

/*@end@*/ 

#ifdef __cplusplus
}
#endif

#endif
