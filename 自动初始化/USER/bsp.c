#include "bsp.h"
#include "auto.h"
#include "stdio.h"

typedef void (*cb)();
cb reg_cb = 0;
static void ctl_led(char state)
{
    state? printf("bsp led on\n") : printf("bsp led off");
}
static void reg_int(void* fn)
{
    reg_cb = (cb)fn;
}
static const io_opt_t led = {
    .set_io = ctl_led,
    .reg = reg_int,
};

/* 创建一个led驱动 */
CREAT_DRIVER(led1) = {
    .name = "io_led",
    .opt = (void*)&led,
};
