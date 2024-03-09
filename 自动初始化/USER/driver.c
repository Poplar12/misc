#include "auto.h"
#include "stdio.h"

static int contrl(unsigned int addr, void *buff, int len)
{
    const char *p = (char*)buff;
    if(*p == 1)
    {
        printf("led on\n");
    }
    else
    {
        printf("lend off\n");
    }
    return 1;
}
CREAT_DEVICE(led) = {
    .name = "led",
    .write = contrl,
};

static int get(unsigned int addr, void *buff, int len)
{
    char* state = (char*)buff;
    *state = 1;
    return 1;
}
CREAT_DEVICE(key) = {
    .name = "key",
    .read = get,
};
