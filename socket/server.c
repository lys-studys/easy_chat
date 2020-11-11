/*************************************************************************
	> File Name: server.c
	> Author:liangyunsong 
	> Mail:1820202749@qq.com 
	> Created Time: 2019年10月18日 星期五 17时01分53秒
 ************************************************************************/

#include"common.h"

int main( int argc, char **argv ){
    int port,server_listen,sockfd;
    struct sockaddr_in client_addr;//第二个结构体 
    pid_t pid;
    socklen_t len = sizeof(client_addr);//accept 函数的一个参数类型
    if(argc != 2){
        printf("Usage : ./server port\n");
        exit(1);
    }
    port = atoi(argv[1]);//把类型转化为整形的函数
    if((server_listen = socket_create(port)) < 0){
        exit(2);
    }

    while(1){

    //阻塞了
    if(( sockfd = accept(server_listen ,(struct sockaddr *)&client_addr , &len))< 0 ){
        perror("accept ");
        continue;    
        }
        if((pid = fork())< 0 ){
            perror(" fork");
            exit(1);
        }
        if(pid == 0){
            close (server_listen);
            char name[50] = {0};
            char buff[512] = {0};
            recv(sockfd,name,50, 0);

            printf(" login %s on %s : %d\n",name , inet_ntoa(client_addr.sin_addr),client_addr.sin_port );
            while(1){
                if(recv( sockfd ,buff, 512, 0)<=0){
                    close(sockfd);
                    break;
                }else if(buff[0]=='#'){
                    FILE *fp;
                    //scanf("%s",buff+1);
                    fp = fopen(buff+1,"r");
                    if(fp == NULL){
                    perror(" fp");
                    continue;
                    }
                    char ch;
                    while((ch = fgetc(fp)) != EOF)
                    putchar(ch);
                    fclose(fp);

                    
                } 
                else{
                    for(int i = 0;  buff[i]; i++){
                        buff[i] -= 32;
                    }
                    printf(" info %s : %s\n ",name ,buff);
                }
            }
        }else {
            close(sockfd);
        }
    }
    
    return 0;
}
