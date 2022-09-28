#include "kernel/types.h"
#include "user/user.h"

#define INT_SIZE sizeof(int)
#define MSGSIZE 35


// 把新建子进程、pipe建立连接和筛选质数进行封装

// 记录一下bug：管道是父进程子进程各自都有写读端，一定要注意在每个进程里都要去关闭啊
void
pipeflow(int p[2], int startnum){

  // 终止条件
  if(startnum == 0){
    exit(0);
  }




  // p是需要读的管道，p2是需要写的管道
  close(p[1]);
  int p2[2];
  printf("prime %d\n", startnum);
  pipe(p2);
  int data;
  int flag = 0;
  int new_startnum = 0;
  // 从p管道读取数据
  while (read(p[0], &data, sizeof(int)) == sizeof(int)) {
    // 将无法整除的数据传递入p2管道
    if (data % startnum){
      write(p2[1], &data, sizeof(int));
      //获取下一轮的起始数据
      if(!flag){
        // printf("new_startnum is %d", data);
        flag = 1;
        new_startnum = data;
      }
    }
    // printf("data is %d\n", data);
  }

  // printf("hello");

  close(p[0]);
  close(p2[1]);


  int ret = fork();

  if (ret > 0) {
    close(p2[0]);
    wait(0);

  } else if (ret == 0) { 
    // 得到buf中的内容，如果还要筛，就继续去fork，父进程也随之回收
    pipeflow(p2,new_startnum); // 从3开始
  }
  exit(0);
}

int
main(int argc, char *argv[])
{
    /*
    质数筛代码：
    int isnp[35]; // is not prime: 不是素数
    for (int i = 2; i * i <= num; i++)
        if (!isnp[i])
            for (int j = i * i; j <= n; j += i)
                isnp[j] = 1;
    */

   // 这题相当于质数筛每一步都用管道传一下
   // 考虑利用fork递归，对于子进程，如果还没筛完就继续fork，
   // 然后及时的把之前的进程终结

  int p[2];
  pipe(p);

  // 第一次
  printf("prime 2\n");
  for(int i = 3; i <= MSGSIZE; i ++){
    if(i % 2 != 0){
        write(p[1], &i, INT_SIZE); // 先筛一遍2的
      }
  }

  int ret = fork();

  if (ret > 0) { 
    // 回收管道资源
    close(p[1]);
    close(p[0]);
    wait(0); // 等待子进程创建
  } else if (ret == 0) { 
    // 得到buf中的内容，如果还要筛，就继续去fork，父进程也随之回收
    pipeflow(p,3); // 从3开始
  }

  exit(0);

}
