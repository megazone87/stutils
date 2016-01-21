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

#ifndef  _ST_MACRO_H_
#define  _ST_MACRO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <strings.h>
#include <sys/time.h>

#define GIT_COMMIT "0"

#define MAX_LINE_LEN        4096
#define MAX_DIR_LEN         256
#define MAX_NAME_LEN        64

#ifndef bool
#define bool char
#define true 1
#define false 0
#endif

#define bool2str(b) ((b)?"true":"false")
#define str2bool(s) ((strncasecmp((s), "true", 4) == 0)? true : false)

#ifndef uint
typedef unsigned int uint;
#endif

#define TIMEDIFF(s, e) (((e).tv_sec - (s).tv_sec)*1000 \
        + ((e).tv_usec - (s).tv_usec)/1000)

#define UTIMEDIFF(s, e) (((e).tv_sec - (s).tv_sec)*1000*1000 \
        + ((e).tv_usec - (s).tv_usec))

#define min(X,Y) (((X)<(Y)) ? (X) : (Y))
#define max(X,Y) (((X)>(Y)) ? (X) : (Y))

#define safe_free(ptr) do {\
    if((ptr) != NULL) {\
        free(ptr);\
        (ptr) = NULL;\
    }\
    } while(0)

#define safe_delete(ptr) do {\
    if((ptr) != NULL) {\
        delete(ptr);\
        (ptr) = NULL;\
    }\
    } while(0)

#define safe_fclose(fp) do {\
    if((fp) != NULL) {\
        fclose(fp);\
        (fp) = NULL;\
    }\
    } while(0)

#define safe_close(fd) do {\
    if((fd) >= 0) {\
        close(fd);\
        (fd) = -1;\
    }\
    } while(0)

/*@ignore@*/ 
#ifdef _ST_NO_CHECK_PARAM_
#define ST_CHECK_PARAM_VOID(cond) 
#define ST_CHECK_PARAM_VOID_EX(cond, fmt, ...)
#define ST_CHECK_PARAM(cond, ret) 
#define ST_CHECK_PARAM_EX(cond, ret, fmt, ...)
#else
#define ST_CHECK_PARAM_VOID(cond) \
    if(cond) \
    {\
        ST_WARNING("Wrong param to %s. ", __func__);\
        return;\
    }
    
#define ST_CHECK_PARAM_VOID_EX(cond, fmt, ...) \
    if(cond) \
    {\
        ST_WARNING("Wrong param to %s. " fmt, __func__, ##__VA_ARGS__);\
        return;\
    }

#define ST_CHECK_PARAM(cond, ret) ST_CHECK_PARAM_EX(cond, ret, "")

#define ST_CHECK_PARAM_EX(cond, ret, fmt, ...) \
    if(cond) \
    {\
        ST_WARNING("Wrong param to %s. " fmt, __func__, ##__VA_ARGS__);\
        return ret;\
    }
#endif
/*@end@*/ 
    
#ifdef __cplusplus
}
#endif

#endif
