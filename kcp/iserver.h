#ifndef __KCP_ISERVER_H__
#define __KCP_ISERVER_H__

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "ikcp.h"


#define ISLEEP_MS 1
#define BUFF_LEN 512
#define WINDOW_SIZE 1024

typedef struct{
    int conv;
    unsigned char *ip;
    int port;
    int sock;
    struct sockaddr_in addr;
    struct sockaddr_in client_addr;

    ikcpcb *pkcp;
    char r_buff[BUFF_LEN];
    char w_buff[BUFF_LEN];
}receiver;


#endif
