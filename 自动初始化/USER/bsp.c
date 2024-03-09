#include "bsp.h"
#include "auto.h"
#include "stdio.h"

static void ctl_led(char state)
{
    state? printf("bsp led on\n") : printf("bsp led off");
}

static const io_opt_t led = {.set_io = ctl_led};

/* ����һ��led���� */
CREAT_DRIVER(led1) = {
    .name = "io_led",
    .opt = (void*)&led,
};
