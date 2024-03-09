#include "auto.h"
#include "string.h"
#include "stdio.h"

/* 设定设备存放位置 */
DEVICE_CREAT(device_start, "0");
DEVICE_CREAT(device_end, "1");
/* 通过名字查找设备 */
const device_t* find_device(const char* name)
{
    const device_t* ptr;
    for(ptr = &device_start + 1; ptr != &device_end; ptr++)
    {
        if(strcmp(ptr->name, name) == 0)
        {
            return ptr;
        }
    }
    return 0;
}

/* 设定驱动存放位置 */
DRIVER_CREAT(driver_start, "0");
DRIVER_CREAT(driver_end, "1");
/* 通过名字查找驱动 */
const driver_t* find_driver(const char* name)
{
    const driver_t* ptr;
    for(ptr = &driver_start + 1; ptr != &driver_end; ptr++)
    {
        if(strcmp(ptr->name, name) == 0)
        {
            return ptr;
        }
    }
    return 0;
}

/* 未来打算再添加一个外设初始化的组, 或者干脆不适用, 直接在函数开头加关键字, 让他自动初始化 */
