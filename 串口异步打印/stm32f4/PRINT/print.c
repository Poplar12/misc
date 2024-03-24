#include "print.h"
#include "que.h"
#include "stdio.h"
#include "stdarg.h"

#define BUF_SIZE   100
creat_que(TX_buf, BUF_SIZE);

transmit enable_transmit = 0;

static void add_str(char *buf, unsigned int len)
{
    int i;

    for(i = 0; i < len; i++) {
        InQue(TX_buf, *(buf + i));
    }

    if(enable_transmit)
    {
        enable_transmit();
    }
}

int my_printf(const char *fmt, ...)
{
    va_list argptr;
    static char buff[BUF_SIZE / 2]; /* 如果是裸机可以使用临时变量 */
    int len = 0;

    /* 按格式将内容打印到buff中 */
    va_start(argptr, fmt);
    len = vsnprintf(buff, BUF_SIZE / 2, fmt, argptr);
    va_end(argptr);

    /* 将内容拷贝到发送队列中 */
    add_str(buff, len);
    return len;
}

/* 从发送队列中获取一个数据 */
int read_data(unsigned char *data)
{
    return OutQue(TX_buf, data);
}

void reg_transmit(transmit cb)
{
    enable_transmit = cb;
}
