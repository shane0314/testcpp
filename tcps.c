#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
 
#define MYPORT  8888
#define QUEUE   20
#define BUFFER_SIZE 1024
 
int main()
{
    ///定义sockfd
    int server_sockfd = socket(AF_INET,SOCK_STREAM, 0);
    printf("socket success, sockfd = %d\n", server_sockfd);
 
    ///定义sockaddr_in
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(MYPORT);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
 
    ///bind，成功返回0，出错返回-1
    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1)
    {
        perror("bind");
        exit(1);
    }
    puts("bind success");
 
    ///listen，成功返回0，出错返回-1
    if(listen(server_sockfd,QUEUE) == -1)
    {
        perror("listen");
        exit(1);
    }
    puts("listen success");
 
    ///客户端套接字
    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);
 
    ///成功返回非负描述字，出错返回-1
    int conn = accept(server_sockfd, (struct sockaddr*)&client_addr, &length);
    if(conn<0)
    {
        perror("connect");
        exit(1);
    }
    puts("accept success");
 
    char sendbuf[BUFFER_SIZE];//
    int i;
    while(1)
    {
        memset(buffer,0,sizeof(buffer));
        int len = recv(conn, buffer, sizeof(buffer),0);
        if( len <= 0 )
        {
            puts("disconnect");
            break;
        }
        printf("\nrecv data length %d :", len);
        if(strcmp(buffer,"exit\n")==0)
            break;
        fputs(buffer, stdout);
        for( i=0; i<len; i++)
        {
            sendbuf[i] = buffer[i] <= 'z' && buffer[i] >= 'a' ? sendbuf[i] = buffer[i] - 'a' + 'A' : buffer[i];
        }
        //sendbuf[i] = '\0';
        send(conn, sendbuf, len, 0);
        memset(buffer,0,sizeof(sendbuf));
    }
    close(conn);
    close(server_sockfd);
    return 0;
}