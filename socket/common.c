/*************************************************************************
	> File Name: common.c
	> Author:liangyunsong 
	> Mail:1820202749@qq.com 
	> Created Time: 2019年10月18日 星期五 16时26分03秒
 ************************************************************************/

#include "common.h"

int socket_create( int port ){
    int socketfd;
    struct sockaddr_in sock_addr;
    //创建套接字 

    if((socketfd = socket(AF_INET,SOCK_STREAM, 0))<0){
        perror("socket ");
        return -1;
    }
    //初始化第二个结构体
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);//字节序转换
    sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//绑定ip地址及端口
    if(bind(socketfd, (struct sockaddr *)&sock_addr,sizeof(sock_addr))< 0){
        perror("bind");
        close(socketfd);
        return -1;
    }
    //监听socket

    if(listen(socketfd, 20)< 0){
        perror(" listen ");
        close(socketfd);
        return -1;

    }

    return socketfd;
}


/*int main(int argc,char *argv){

    
    
    return 0;
}*/

