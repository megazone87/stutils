#ifndef _ST_LOG_H_
#define _ST_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define ST_LOG_LEV_CLEANEST     0x01
#define ST_LOG_LEV_CLEANER      0x02
#define ST_LOG_LEV_CLEAN        0x03
#define ST_LOG_LEV_FATAL	    0x04
#define ST_LOG_LEV_WARNING	    0x05
#define ST_LOG_LEV_NOTICE       0x06
#define ST_LOG_LEV_TRACE	    0x07
#define ST_LOG_LEV_DEBUG	    0x08


/*@ignore@*/ 
#define ST_LOG(lev, fmt, ...) \
    st_writelog(lev, "[%s:%d<<%s>>] " fmt, __FILE__, __LINE__, __func__, \
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
    st_writelog(ST_LOG_LEV_CLEANEST, fmt, ##__VA_ARGS__);

#define ST_CLEANER(fmt, ...) \
    st_writelog(ST_LOG_LEV_CLEANER, fmt, ##__VA_ARGS__);

#define ST_CLEAN(fmt, ...) \
    st_writelog(ST_LOG_LEV_CLEAN, fmt, ##__VA_ARGS__);

int st_openlog(const char *log_file, int mask);

int st_openlog_mt(const char *log_file, int mask);

int st_writelog(const int lev, const char* fmt, ... );

int st_closelog(int err);

/*@end@*/ 

#ifdef __cplusplus
}
#endif

#endif
