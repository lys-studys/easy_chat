/*************************************************************************
	> File Name: receive.c
	> Author:liangyunsong 
	> Mail:1820202749@qq.com 
	> Created Time: 2020年04月22日 星期三 10时41分52秒
 ************************************************************************/

#define MAX_SEQ 1
typedef enum { frame_arrival, chksum_err, timeout  } event_type; 
#include "protocol.h"
#include <errno.h>
#include <string.h>
//#include <math.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
void InitSocket ( void  );
void Receiver ( void  );
/*************************************************************************
 *   *      函数名:          InitSocket
 *     *      参数:            NIL
 *       *      返回值:          Void
 *         *      描述:            socket通信初始化
 *         ***************************************************************************/
 void InitSocket ( void  )
 {
            int CliLen;
            printf (  "输入帧的数目 : "  );
            scanf ( "%d", &NumOfFrames  );
            printf ( "输入端口号 : "  );
            scanf ( "%d", &Port  );
            printf ( "\n"  );
            if ( (newsockfd = socket ( AF_INET, SOCK_STREAM, 0  )) < 0  )
     {
                       perror ( "服务器 : 无法打开Socket\n"  );
                       exit (1);
                
     }
            bzero ( (char *) &Server, sizeof (Server)  );
            Server.sin_family = AF_INET;
            Server.sin_addr.s_addr = htonl (INADDR_ANY);
            Server.sin_port = htons (Port);
            if ( bind ( newsockfd, (struct sockaddr *) &Server, sizeof (Server)  ) < 0  )
     {
                       perror ( "服务器 : 无法绑定本地地址\n"  );
                       exit (1);
                
     }
            listen ( newsockfd, 1  );
            printf ( "等待..........\n\n"  );
            CliLen = sizeof (Client);
            sockfd = accept ( newsockfd, (struct sockaddr *) &Client, &CliLen  );
            printf ( "成功接受客户端....\n\n"  );

 }
 /*************************************************************************
  *   *      函数名:           Sender
  *     *      参数:             NIL
  *       *      返回值:           Void
  *         *      描述:             从发送方接收帧，并发送带ack的帧给发送方
  *         ***************************************************************************/
  void Receiver ( void  )
  {
             seq_nr frame_expected;
             frame r, s;
             event_type event;
             int i;
             s.kind = ack;
             s.seq = 0;
             s.ack = 1;
             strcpy ( s.info.data, ""  );
             frame_expected = 0;
             for ( i = 0; i < NumOfFrames; i++  )
      {
                        wait_for_event (&event);
                        if ( event == frame_arrival  )
          {
                                   from_physical_layer (&r);
                                   printf ( "接收帧 (数据)  %d ......\n", r.seq  );
                                   if ( r.seq == frame_expected  )
              {
                                             to_network_layer (&r.info);
                                             inc (frame_expected);
                                       
              }
                                   printf ( "发送ACK ......\n"  );
                                   to_physical_layer (&s);
                                   sleep (1);
                            
          }
                        printf ( "\n"  );
                 
      }

  }
  /*************************************************************************
   *   *      函数名:           main
   *     *      参数:             NIL
   *       *      返回值:           NIL
   *         *      描述:             主函数
   *         ***************************************************************************/
int  main ()
   {
              system ( "clear"  );
              InitSocket ();
              enable_network_layer ();
              Receiver ();

   }

