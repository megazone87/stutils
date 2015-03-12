#ifndef _ST_NET_H_
#define _ST_NET_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <arpa/inet.h>

#define ST_NET_INTERNEL_ERROR     -1
#define ST_NET_CONNECT_TIMEOUT    -2
#define ST_NET_SND_RCV_TIMEOUT    -3
#define ST_NET_NETWORK            -4

#ifdef __cplusplus
extern "C" {
#endif

int st_listen(int port, int queue);

int st_connect(const char* addr, short port, int *tmo);

int st_read(int fd, int *tmo, void* buf, size_t len);

int st_write(int fd, int *tmo, void* buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif

