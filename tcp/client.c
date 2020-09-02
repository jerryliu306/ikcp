#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>


//定义连接服务器IP，根据服务端IP地址修改
char * IPADDRESS = NULL;

//定义连接服务器端口，根据服务端监听端口修改
#define SERV_PORT 8888
#define MAXSIZE 1024

int N = 300;

static int iclock()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000 + tv.tv_usec/1000;  //毫秒
}


int main(int argc, char *argv[])
{

    int sockfd;
    struct sockaddr_in servaddr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);//创建socket
    memset(&servaddr, 0, sizeof(servaddr));

    IPADDRESS = argv[1];
    N = atoi(argv[2]);

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, IPADDRESS, &servaddr.sin_addr);
    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));//连接socket

    char tmp[460];
    memset(tmp, 0, 460);
    int i=0;
    for(i=0; i<459; i++) {
        tmp[i] = 'A' + i%26;
    }

    for (i=0; i<N; i++) {
        char buf[MAXSIZE] = {0,};
        sprintf(buf, "TCP TEST, data=%s|||id=%d", tmp, i);
        printf("send msg:%s\n", buf);

        int start_time = iclock();
        write(sockfd, buf, strlen(buf));

        printf("write done...\n");

        memset(buf, 0, sizeof(buf));
        read(sockfd, buf, MAXSIZE);
        printf("read done...\n");

        int end_time = iclock();
        printf("read msg:%s|||||| cost=%d\n", buf, end_time-start_time);

        if (strcmp(buf, "q") == 0) {
            printf("close client!\n");
            close(sockfd);
            break;
        }
    }
    return 0;
}
