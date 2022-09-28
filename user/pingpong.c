#include "kernel/types.h"
#include "user/user.h"

#define MSGSIZE 32 // 设置缓冲区

int
main(int argc, char *argv[])
{
/* 子进程读管道，父进程写管道 */
  int p1[2];
/* 父进程读管道，子进程写管道 */
  int p2[2];

  char inbuf[MSGSIZE];

  pipe(p1);
  pipe(p2);

  int ret = fork();

  if (ret == 0) { 
    /* 子进程 */
    close(p1[1]); // 关闭写端
    read(p1[0],inbuf,MSGSIZE);
    printf("%d: received %s", getpid(),inbuf);
    close(p1[0]); // 读取完成，关闭读端

    // 读完以后再传回去pong
    close(p2[0]); // 关闭读端
    write(p2[1], "pong\n", MSGSIZE);
    close(p2[1]); // 写入完成，关闭写端


  } else if (ret>0) { 
    /* 父进程 */
    close(p1[0]); // 关闭读端
    write(p1[1], "ping\n", MSGSIZE);
    close(p1[1]); // 写入完成，关闭写端

    // 开始接收
    close(p2[1]);
    read(p2[0],inbuf,MSGSIZE);
    printf("%d: received %s", getpid(),inbuf);
    close(p2[0]);


  }

  exit(0);

}
