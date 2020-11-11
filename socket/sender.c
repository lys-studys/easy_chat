/*************************************************************************
	> File Name: sender.c
	> Author:liangyunsong 
	> Mail:1820202749@qq.com 
	> Created Time: 2020年04月22日 星期三 10时39分37秒
 ************************************************************************/
#define MAX_SEQ 1
typedef enum { frame_arrival, chksum_err, timeout } event_type;
#include "protocol.h"
#include <errno.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
void InitSocket ( void );
void Sender ( void );
/*************************************************************************
  *      函数名:          InitSocket
  *      参数:            NIL
  *      返回值:          Void
  *      描述:            socket通信初始化
***************************************************************************/
void InitSocket ( void )
{
       printf ( "输入帧的数目 : " );
       scanf ( "%d", &NumOfFrames );
       printf ( "输入端口号 : " );
       scanf ( "%d", &Port );
       printf ( "\n" );
       if ( (sockfd = socket ( AF_INET, SOCK_STREAM, 0 )) < 0 )
       {
              perror ( "客户端 : 无法打开Socket\n" );
              exit (1);
       }
       bzero ( &Client, sizeof (Client) );
       Client.sin_family = AF_INET;
       Client.sin_addr.s_addr = inet_addr ( ServerIP );
       Client.sin_port = htons (Port);
       printf ( "等待..........\n\n" );
       if ( connect ( sockfd, (struct sockaddr *) &Client, sizeof (Client) ) < 0 )
       {
              perror ( "客户端 : 无法连接到服务器\n" );
              exit (1);
       }
       printf ( "客户端成功连接到服务器.....\n\n" );
}
 
/*************************************************************************
  *      函数名:           Sender
  *      参数:             NIL
  *      返回值:           Void
  *      描述:             将帧发送给接收方，并从接收方接收传回的帧
***************************************************************************/
void Sender ( void )
{
       seq_nr next_frame_to_send;
       frame s, r;
       packet buff;
       event_type event;
       int i;
       next_frame_to_send = 0;
       from_network_layer (&buff);
       s.kind = data;
       s.ack = 0;
 
       for ( i = 0; i < NumOfFrames; i++ )
       {
              s.info = buff;
              s.seq = next_frame_to_send;
 
              printf ( "发送帧 (数据) %d ......\n", s.seq );
              to_physical_layer (&s);
              if ( i == NumOfFrames - 1 )
                     break;
              start_timer (s.seq);
              sleep (1);
              wait_for_event (&event);
              if ( event == frame_arrival )
              {
                     printf ( "接收帧 (ACK) %d ......\n\n", s.seq );
                     from_physical_layer (&r);
                     from_network_layer (&buff);
                     inc (next_frame_to_send);
              }
       }
       wait_for_event (&event);
       if ( event == frame_arrival )
              printf ( "接收帧 (ACK) %d ......\n\n", s.seq );
}
/*************************************************************************
  *      函数名:           main
  *      参数:             NIL
  *      返回值:           NIL
  *      描述:             主函数
***************************************************************************/
int main ()
{
       system ( "clear" );//清屏
       InitSocket ();//初始化socket
       enable_network_layer ();//启用网络层
       Sender ();


}
