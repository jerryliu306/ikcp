#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define PORT 8888
#define MAXSIZE 1024

int main(int argc, char const *argv[])
{
    //创建socket
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        printf("create socket error!\n");
        return -1;
    }

    //构造地址
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORT);
    saddr.sin_addr.s_addr = INADDR_ANY;

    //使用上面构造的地址绑定socket
    if (bind(sfd, (struct sockaddr *)&saddr, sizeof(struct sockaddr)) == -1) {
        printf("bind socket error!, errno=%s\n", strerror(errno));
        return -1;
    }

    //监听socket
    if (listen(sfd, 8) == -1) {
        printf("listen error!\n");
        return -1;
    }

    //构造地址用于接受socket后写入客户端地址
    struct sockaddr_in peer_saddr;
    socklen_t peer_len = sizeof(struct sockaddr);
    memset(&peer_saddr, 0, sizeof(peer_saddr));

    //阻塞直到收到客户端发送连接请求，接受请求
    int client = accept(sfd, (struct sockaddr *)&peer_saddr, &peer_len);
    if (client == -1) {
        printf("accept error!,errno=%s\n", strerror(errno));
        return -1;
    }
    else {
        printf("accept client: %s:%d\n", inet_ntoa(peer_saddr.sin_addr), peer_saddr.sin_port);
    }

    //与客户端通讯
    for (;;) {
        char buffer[MAXSIZE] = {0};
        read(client, buffer, MAXSIZE);

        printf("read:%s\n", buffer);
        write(client, buffer, strlen(buffer));
        if (strcmp(buffer, "q") == 0) {
            printf("close server!\n");
            close(client);
            break;
        }
    }
    close(sfd);
    return 0;
}
