#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

#define MSGSIZE 32 // 设置缓冲区


// 记录一下bug，就是我直接调用了exec()，但是应该要fork以后再调用
// 不然的话主进程就直接被替换然后噶了

int
main(int argc, char  *argv[])
{

    // 获取前一个命令的输出作为输入参数的前半段
    char inbuf[MSGSIZE];
    read(0, inbuf, MSGSIZE);
    // printf("%s", inbuf);

    // 把xargs命令的参数和之前的参数拼起来，作为新的参数
    char *xargsv[MAXARG]; // 指导书提示的常量

    int xargsc = 0;

    // argv第一个元素是命令的名称，之后都是参数
    for (int i = 1 ; i < argc; i++){
        
        xargsv[i-1] = argv[i];
        xargsc += 1;
        // printf("xargsv[%d] is %s\n", i-2 , xargsv[i-2]);
    }
 //echo 22 | xargs echo 556 45

    char *start = inbuf;

    // 以\n为分界标志，每轮都去找'\n'然后把上一个输入拼起来

    for (int i = 0; i < MSGSIZE; i++){
        if(inbuf[i] == '\n'){

            if(fork() == 0){
                // printf("haoye\n");
                inbuf[i] = 0; // 终结标志
                xargsv[xargsc++] = start;
                xargsv[xargsc++] = 0;
                // printf("xargsv[2] is %s\n",xargsv[2]);

                // exec是从index为1开始取参数的
                exec(argv[1], xargsv);
            }
            else{
                start = &inbuf[i + 1];
                wait(0);
            }

    }
    }

    exit(0);
}
