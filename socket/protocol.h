/*************************************************************************
	> File Name: protocol.h
	> Author:liangyunsong 
	> Mail:1820202749@qq.com 
	> Created Time: 2020年04月22日 星期三 10时38分06秒
 ************************************************************************/

#ifndef _PROTOCOL_H
#define _PROTOCOL_H
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/socket.h>
#include <unistd.h>
#define MAX_PKT 12
#define BUF_SIZE 18 
#define ServerIP "192.168.0.105"
#define inc(k) if (k < MAX_SEQ) k = k + 1; else k = 0
typedef unsigned int seq_nr;
typedef enum { false, true } boolean;   
typedef enum { data, ack, nak } frame_kind;     
/* --- 定义包结构 --- */
typedef struct
{
       unsigned char data[MAX_PKT];
} packet;     
/* --- 定义帧结构 --- */
typedef struct
{      
       frame_kind kind;     
       seq_nr seq;          
       seq_nr ack;          
       packet info;         
} frame;
 
/* --- 全局变量声明 --- */
struct sockaddr_in Server, Client;
int sockfd, newsockfd;
int Port, NumOfFrames = 10;
char buffer[BUF_SIZE];
int NETWORK_LAYER, Index = 0;
time_t timer[MAX_SEQ];
/* ----- ARQ协议函数 ----- */
void wait_for_event ( event_type *event );
void Dec_Bin ( int Index, int Bin[] );
void generate_ham_code ( char data[], char ham_code[] );
int check_sum ( char ham_code[] );
void from_network_layer ( packet *p );
void to_network_layer ( packet *p );
void from_physical_layer ( frame *r );
void to_physical_layer ( frame *s );
void start_timer ( seq_nr k );
void stop_timer ( seq_nr k );
void start_ack_timer ( void );
void stop_ack_timer ( void );
void enable_network_layer ( void );
void disable_network_layer ( void );
/* -- 事件类型 -- */
void wait_for_event ( event_type *event )
{
       fd_set rfds, wfds;
       time_t t;
       int i;
       FD_ZERO (&rfds);
       FD_SET ( sockfd, &rfds );
       FD_ZERO (&wfds);
       FD_SET ( sockfd, &wfds );
       time (&t);
       for ( i = 0; i < MAX_SEQ; i++ )
              if ( timer[i] != 0 && t - timer[i] > 5 )
              {
                     printf ( "事件 : 超时\n" );
                     *event = timeout;
                     return;
              }
       if ( select ( sockfd+1, &rfds, &wfds, NULL, NULL) != -1 )
       {
              printf ( "事件 : 帧到达\n" );
              *event = frame_arrival;
              return;
       }
}
 
/* --- 十进制转二进制 --- */
void Dec_Bin ( int Index, int Bin[] )
{
       int i = 0, Num = Index, len = (int) (log (MAX_PKT)/log (2))+1;
       for ( i = 0; i < len; i++ )
              Bin[i] = 0;
       i = 0;
       while ( Num != 0 )
       {
              Bin[len-1-i++] = Num % 2;
              Num = Num / 2;
       }
}
 
/* --- 生成汉明码 --- */
void generate ( char data[], char ham_code[] )
{
       int i, j, k, l;
       int bin[(int) (log (MAX_PKT)/log (2))+1];
       for ( i = 0, j = 1, k = 0; i < MAX_PKT-1; i++ )
              if ( i == j - 1 )
              {
                     ham_code[i] = '0';
                     j *= 2;
              }
              else
                     ham_code[i] = data[k++];
       for ( i = 0, j = 0; i < MAX_PKT-1; i++ )
              if ( i == (int) pow ( 2, j ) - 1 )
                     j++;
              else  if ( ham_code[i] == 1 + '0' )
                     {
                            Dec_Bin ( i+1, bin );
                           for ( k = 0, l = 1; k < (int) (log (MAX_PKT)/log (2)) + 1; k++, l *= 2 )
                                  if ( bin[k] == 1 )
                                         ham_code[l - 1] = '0' + 1 + '0' - ham_code[l - 1];
                     }
}
/* --- CRC验证 --- */
int check_sum ( char ham_code[] )
{
       char chk_ham_code[MAX_PKT], data[MAX_PKT];
       int i, j, k;
       memset ( chk_ham_code, '\0', MAX_PKT );
       memset ( data, '\0', MAX_PKT );
       for ( i = 0, j = 0, k = 1; i < MAX_PKT-1; i++ )
              if ( i == k - 1 )
                     k *= 2;
              else
                     data[j++] = ham_code[i];
       generate ( data, chk_ham_code );
       if ( strcmp ( ham_code, chk_ham_code ) == 0 )
              return 1;
       else
              return 0;
}
/* --- 从网络层接收一个包 --- */
void from_network_layer ( packet *p )
{
       int i, RandomNumber;
       char ham_code[MAX_PKT];
       memset ( p->data, '\0', MAX_PKT );
       memset ( ham_code, '\0', MAX_PKT );
       if (NETWORK_LAYER)
       {
              printf ( "从网络层 : " );
              for ( i = 0; i < MAX_PKT - (int) (log (MAX_PKT)/log (2)) - 2; i++ )
              {
                     RandomNumber = rand () % 2;
                     p->data[i] = '0' + (char) RandomNumber;
              }
              generate ( p->data, ham_code );
              strcpy ( p->data, ham_code );
              printf ( "%s\n", p->data );
              if ( (RandomNumber = rand () % 4) == 0 )
                     p->data[(int) pow ( 2, RandomNumber ) - 1] = '0' + 1 + '0' - p->data[(int) pow ( 2, RandomNumber ) - 1];
       }
}
/* --- 将包发送到网络层 --- */
void to_network_layer ( packet *p )
{
       int i;
       if (NETWORK_LAYER)
       {
              printf ( "到网络层 : " );
              for ( i = 0; i < MAX_PKT; i++ )
                     printf ( "%c", p->data[i] );
              printf ( "\n" );
              if ( check_sum (p->data) == 0 )
                     printf ( "Check sum error\n" );
       }
}
/* --- 从物理层接收一个包 --- */
void from_physical_layer ( frame *r )
{
    memset ( buffer, '\0', BUF_SIZE );
    recv ( sockfd, &buffer, BUF_SIZE-1, 0 );
    sscanf ( buffer,"%u %u %u %s", &r->kind, &r->seq, &r->ack, r->info.data );
}
/* --- 发送一个包到物理层 --- */
void to_physical_layer ( frame *s )
{
    memset ( buffer, '\0', BUF_SIZE );
    sprintf ( buffer, "%d %d %d %s", s->kind, s->seq, s->ack, s->info.data );
    send ( sockfd, &buffer, strlen (buffer), 0 );
}
 
/* --- 开启普通帧定时器 --- */
void start_timer ( seq_nr k )
{
       time_t t;
       timer[k % MAX_SEQ] = time (&t);
}
/* --- 停止普通帧定时器 --- */
void stop_timer ( seq_nr k )
{
       timer[k % MAX_SEQ] = 0;
}
/* --- 开启ACK定时器 --- */
void start_ack_timer ( void )
{
}
/* --- 停止ACK定时器 --- */
void stop_ack_timer ( void )
{
}
/* --- 启用网络层 --- */
void enable_network_layer ( void )
{
       srand (time (NULL));
       NETWORK_LAYER = 1;
}
/* --- 禁用网络层 --- */
void disable_network_layer ( void )
{
       NETWORK_LAYER = 0;
}

#endif
