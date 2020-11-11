/********************* * ***************************************************
	> File Name: client.c
	> Author:liangyunsong 
	> Mail:1820202749@qq.com 
	> Created Time: 2019年10月13日 星期日 20时16分59秒
 ************************************************************************/

#include"common.h"
#include "head.h"
int main(int argc,char **argv){
    int sockfd;
    struct timeval tm;
    fd_set wfds;
    unsigned long len = 1;
    int len_t = sizeof(int);
    char buff[512];
    char *server = argv[1];//ip地址
    int port =atoi(argv[2]);//端口
    char *name = argv[3];
    struct sockaddr_in server_addr;//对端的端口ip
    if((sockfd = socket(AF_INET,SOCK_STREAM,0))<0){
        perror("socket()");
        exit(1);
    }
    ioctl(sockfd, FIONBIO, &len);
    FD_ZERO(&wfds);
    FD_SET(sockfd, &wfds);
    tm.tv_sec = 0;
    tm.tv_usec = 10000;
    int error = -1;
    server_addr.sin_family = AF_INET;//ipv4
    server_addr.sin_port = htons(port);//转化为网络字节序
    server_addr.sin_addr.s_addr = inet_addr(server);//点分十进制转化为32位无符号整形
   // server_addr.sin_eddr.s_addr = inet_addr(host);
   while(1){ 
    if(connect(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr))<0){
        if(select(sockfd + 1, NULL, &wfds, NULL, &tm) > 0){
            getsockopt(sockfd,SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len_t);
            if(error != 0){
                printf("getsockopt wrong !");
                close (sockfd);
                return -1;
            }
            char *msg = (char *)malloc(sizeof(char));
            strcpy(msg, name);
            send(sockfd, msg, sizeof(msg), 0);
            memset(msg, 0 , sizeof(msg));
            recv(sockfd, msg, sizeof(msg), 0);
            if(*msg == '1'){
                printf("success recv \n");
                memset(name, 0, sizeof(name));
            }
        }
    }
    send(sockfd,name,strlen(name),0);
    while(1){
        scanf("%[^\n]s",buff);
        getchar();
        send(sockfd,buff,strlen(buff), 0);
    }
    close(sockfd);
   }

    return 0;

}

