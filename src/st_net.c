#include "st_net.h"

void update_timeout(int fd, int *tmo, struct timeval* start, struct timeval* end)
{
    struct timeval tv;
    if((start != NULL) && (end != NULL)) {
        *tmo = *tmo - (end->tv_sec - start->tv_sec)*1000 - (end->tv_usec - start->tv_usec)/1000;
    }
    tv.tv_sec = *tmo / 1000;
    tv.tv_usec = (*tmo % 1000)*1000;
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&tv, sizeof(struct timeval));
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval));
}

int st_connect(const char* addr, short port, int *tmo)
{
    struct sockaddr_in server_addr;
    struct hostent* host;
    struct timeval start;
    struct timeval end;
    int tcp_nodelay = 1;
    int fd;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1) {
        return ST_NET_INTERNEL_ERROR;
    }

    if(setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &tcp_nodelay, sizeof(tcp_nodelay)) != 0) {
        return ST_NET_INTERNEL_ERROR;
    }
    update_timeout(fd, tmo, NULL, NULL);

    memset(&server_addr, 0, sizeof(server_addr));

    host = gethostbyname(addr);
    if(!host) {
        return ST_NET_INTERNEL_ERROR;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr*)host->h_addr);

    gettimeofday(&start, NULL);
    if(connect(fd, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr)) != 0) {
        return ST_NET_CONNECT_TIMEOUT;
    }
    gettimeofday(&end, NULL);
    update_timeout(fd, tmo, &start, &end);

    return fd;
}

int st_read(int fd, int *tmo, void* buf, size_t len)
{
    struct timeval start;
    struct timeval end;
    size_t remain_len = len;
    char* read_pos = (char*)buf;
    int timeout = 0;

    while(remain_len > 0) {
        size_t read_len = 0;
        gettimeofday(&start, NULL);
        read_len = read(fd , read_pos, remain_len);
        gettimeofday(&end, NULL);
        if(*tmo - (end.tv_sec - start.tv_sec)*1000 - (end.tv_usec - start.tv_usec)/1000 <= 0) {
            timeout = 1;
        }
        if(read_len <= 0) {
            break;
        }
        read_pos += read_len;
        remain_len -= read_len;

        if(timeout == 1) {
            break;
        }
        update_timeout(fd, tmo, &start, &end);
    }
    if(remain_len > 0) {
        if(timeout == 1) {
            return ST_NET_SND_RCV_TIMEOUT;
        }
        return ST_NET_NETWORK;
    }
    return 0;
}

int st_write(int fd, int *tmo, void* buf, size_t len)
{
    struct timeval start;
    struct timeval end;

    gettimeofday(&start, NULL);
    if(write(fd, buf, len) != (ssize_t)len) {
        gettimeofday(&end, NULL);
        if(*tmo - (end.tv_sec - start.tv_sec)*1000 - (end.tv_usec - start.tv_usec)/1000 <= 0) {
            return ST_NET_SND_RCV_TIMEOUT;
        }
        return ST_NET_NETWORK;
    }
    gettimeofday(&end, NULL);
    update_timeout(fd, tmo, &start, &end);
    return 0;
}

int st_listen(int port, int queue)
{
    int listenfd;
    const int on = 1;
    struct sockaddr_in sin;

    if ((listenfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) 
    {
        return -1;
    }

    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(port);
    if (bind(listenfd, (struct sockaddr *) &sin, sizeof(sin)) < 0) 
    {
        close(listenfd);
        return -1;
    }

    (queue <= 0) ? queue = 5 : queue;
    if (listen(listenfd, queue) < 0) 
    {
        close(listenfd);
        return -1;
    }

    return listenfd;
}

