#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "iserver.h"
#include "common.h"


int handler_server_output(const char *buf, int len, ikcpcb *kcp, void *user){
    receiver *rcver = (receiver*)user;

    int n = 0;
    n = sendto(rcver->sock, buf, len, 0, (struct sockaddr *)&(rcver->client_addr), sizeof(rcver->client_addr));
    if(n<0){
        printf("output=sendto:err");
    }

    printf("output=sendto:%s, len=%d, n=%d\n", buf, len, n);
    return 0;
}


int init(receiver *rcver){ 
    ikcpcb *pkcp1  = ikcp_create(rcver->conv, rcver);
    pkcp1->rx_minrto = 200;

    rcver->pkcp = pkcp1;
    pkcp1->output = handler_server_output;

    ikcp_nodelay(pkcp1, 1, 5, 2, 1);
    ikcp_wndsize(pkcp1, WINDOW_SIZE, WINDOW_SIZE);

    bzero(&(rcver->addr), sizeof(rcver->addr));
    rcver->addr.sin_family = AF_INET;
    rcver->addr.sin_port = htons(8010);
    rcver->addr.sin_addr.s_addr = htonl(INADDR_ANY);
    printf("server ip,port: %s   %d\n", rcver->ip, rcver->port);

    rcver->sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(rcver->sock<0){
        perror("socket error!");
        exit(1);
    }

    if(bind(rcver->sock, (struct sockaddr *)&(rcver->addr), sizeof(rcver->addr))<0){
        perror("bind error!");
        exit(1);
    }

    return 0;
}


int loop(receiver *rcver){
    int n;
    int ret;

    int len = sizeof(struct sockaddr_in);
    while(1){
        memset(rcver->r_buff, 0, BUFF_LEN);
        n = recvfrom(rcver->sock, rcver->r_buff, BUFF_LEN, MSG_DONTWAIT, (struct sockaddr *)&(rcver->client_addr), &len);
        if (n > 0) {
            printf("recved, n=%d, data=%s\n", n, rcver->r_buff);
            printPackage(rcver->r_buff); //TODO DELETE

            ret = ikcp_input(rcver->pkcp, (char*)rcver->r_buff, n);
            if (ret < 0) {
                fprintf(stderr, "ikcp_input error: %d\n", ret);
                continue;
            }
        }

        isleep(ISLEEP_MS);
        ikcp_update(rcver->pkcp, iclock());

        int peek_size = ikcp_peeksize(rcver->pkcp);
        if (peek_size <= 0) {
            continue;
        }

        int msg_id =0;
        int kcp_data_recved = 0;

        char r_user_data[BUFF_LEN];
        memset(r_user_data, 0, BUFF_LEN);
        while(1) {

            n = ikcp_recv(rcver->pkcp, r_user_data, BUFF_LEN);
            if(n<0){
                printf("ikcp_recv error, n=%d\n", n);
                break;
            }

            kcp_data_recved = 1;
            printf("ikcp_recv user data:%s, n=%d\n", r_user_data, n);
            sscanf(r_user_data, "KCP DEMO TEST|||id=%d", &msg_id);
            printf("after sscanf, msg_id=%d", msg_id);
        }

        if (kcp_data_recved ==1 ){
            memset(rcver->w_buff, 0, BUFF_LEN);
            sprintf(rcver->w_buff, "Ok, your msg data:%s, id=%d.", r_user_data, msg_id);
            int n = ikcp_send(rcver->pkcp, rcver->w_buff, strlen(rcver->w_buff));

            isleep(ISLEEP_MS);
            ikcp_update(rcver->pkcp, iclock());
        }
    }
    return 0;
}


int main(int argc, char **argv){

    printf("Udp server start...\n");

    receiver rcver;
    rcver.conv = 0x001;
    rcver.ip = argv[1];
    rcver.port = htons(atoi("8010"));

    init(&rcver);
    loop(&rcver);

    return 0;
}
