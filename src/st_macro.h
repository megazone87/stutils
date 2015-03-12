#ifndef  _ST_MACRO_H_
#define  _ST_MACRO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <sys/time.h>
#include "stlog.h"

#define MAX_LINE_LEN        4096
#define MAX_DIR_LEN         256
#define MAX_NAME_LEN        64

#ifndef bool
#define bool int
#define true 1
#define false 0
#endif

#ifndef uint
typedef unsigned int uint;
#endif

#define TIMEDIFF(s, e) ((e.tv_sec - s.tv_sec)*1000 \
        + (e.tv_usec - s.tv_usec)/1000)

#define UTIMEDIFF(s, e) ((e.tv_sec - s.tv_sec)*1000*1000 \
        + (e.tv_usec - s.tv_usec))

#define min(X,Y) (((X)<(Y))?X:Y)
#define max(X,Y) (((X)>(Y))?X:Y)

#define safe_free(ptr) do {\
    if(ptr != NULL) {\
        free(ptr);\
        ptr = NULL;\
    }\
    } while(0)

#define safe_delete(ptr) do {\
    if(ptr != NULL) {\
        delete(ptr);\
        ptr = NULL;\
    }\
    } while(0)

#define safe_fclose(fp) do {\
    if(fp != NULL) {\
        fclose(fp);\
        fp = NULL;\
    }\
    } while(0)

#define safe_close(fd) do {\
    if(fd >= 0) {\
        close(fd);\
        fd = -1;\
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

