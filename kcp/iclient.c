#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "iclient.h"
#include "ikcp.h"
#include "common.h"


int N = 300;


int handler_client_output(const char *buf, int len, ikcpcb *kcp, void *user){
    sendor *sdor = (sendor*)user;

    uint16_t dest_len = sizeof(struct sockaddr);

    printf("Send data:%s, iclock=%d\n", buf, iclock());

    int n = sendto(sdor->sock_fd, buf, len, 0, (struct sockaddr *)&(sdor->addr_server), sizeof(sdor->addr_server));
    if (n >= 0) {
        return n;
    } else {
        printf("error: %d bytes send, errno:%d\n", n, errno);
        printf("sendto error: %s(errno: %d)\n", strerror(errno), errno);
        return -1;
    }
}


int init(sendor *sdor){
    ikcpcb *kcp1 = ikcp_create(sdor->conv, sdor);
    kcp1->rx_minrto = 200;
    kcp1->output = handler_client_output;

    sdor->pkcp=kcp1;
    ikcp_nodelay(kcp1, 1, 5, 2, 1);
    ikcp_wndsize(kcp1, WINDOW_SIZE, WINDOW_SIZE);

    memset(&(sdor->addr_server), 0, sizeof(sdor->addr_server));

    sdor->addr_server.sin_family = AF_INET;
    sdor->addr_server.sin_addr.s_addr = inet_addr((char*)sdor->ip);
    sdor->addr_server.sin_port = htons(8010);
    printf("server ip,port: %s   %d\n", sdor->ip, sdor->port);

    sdor->sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if(sdor->sock_fd<0){
        printf("socket creation failed\n");
        exit(-1);
    } else { 
        printf("socket creation sucess\n");
    }

    return 0;
}


int loop(sendor *sdor) {
    printf("LOOP---\n");
    int id = 0;

    int kcp_data_recved = 1;
    int start_time = 0;

    char tmp[400];
    memset(tmp, 0, 400);
    int i=0;
    for(i=0; i<399; i++) {
        tmp[i] = 'A' + i%26;
    }

    while(1) {
        char data[BUFF_LEN] = {0,};

        if (kcp_data_recved == 1) {

            sprintf(data, "KCP DEMO TEST,data=%s|||id=%d", tmp, ++id);
            const int data_len = strlen(data);

            memset(sdor->w_buff, 0, BUFF_LEN);
            memcpy(sdor->w_buff, data, data_len);

            start_time = iclock();
            int n = ikcp_send(sdor->pkcp, sdor->w_buff, data_len);
            if(n<0){
                perror("error ikcp_send!");
                break;
            }

            printf("call ikcp_send, iclock=%d\n", iclock());
            isleep(ISLEEP_MS);
            ikcp_update(sdor->pkcp, iclock());
            kcp_data_recved = 0;
        }


        while(1) {

            memset(sdor->r_buff, 0, BUFF_LEN);
            int n = recvfrom(sdor->sock_fd, sdor->r_buff, BUFF_LEN, MSG_DONTWAIT, NULL, NULL);
            if (n < 0) {
                break;
            }

            printPackage(sdor->r_buff);

            printf("Received by recvfrom=%s|||n=%d", sdor->r_buff, n);
            int ret = ikcp_input(sdor->pkcp, sdor->r_buff, n);
            if (ret < 0) {
                fprintf(stderr, "ikcp_input error: %d\n", ret);
            }

            isleep(ISLEEP_MS);
            ikcp_update(sdor->pkcp, iclock());
        }

        while(1){
            int peek_size = ikcp_peeksize(sdor->pkcp);
            if (peek_size <=0 ){
                break;
            }

            char user_data[BUFF_LEN];
            memset(user_data, 0, BUFF_LEN);

            printf("Before ikcp_recv\n");
            int n=ikcp_recv(sdor->pkcp, user_data, BUFF_LEN);

            printf("After ikcp_recv, user_data:%s\n", user_data);
            if(n<0) {
                printf("error, n=%d\n", n);
            }
            int end_time = iclock();
            printf("=============================>>>>user_data=%s, start=%d, end=%d, cost:%d\n", user_data, start_time, end_time, end_time-start_time);
            kcp_data_recved = 1;
        }

        if(id >= N){
            break;
        }
    }

    return 0;
}


int main(int argc,char **argv){
    printf("UDP client start...\n");

    sendor sdor;
    sdor.conv = 0x001;
    sdor.ip = argv[1];
    sdor.port = htons(atoi("8010"));

    N = atoi(argv[2]);

    init(&sdor);
    loop(&sdor);

    return 0;
}
