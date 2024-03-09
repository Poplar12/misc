
#ifndef AUTO_H
#define AUTO_H

/* 驱动结构体: 由于不同的驱动, 需要的接口函数不同, 为了节省空间再封了一层 */
typedef struct
{
    const char* name;
    void* opt;
}driver_t;

/* 设备结构体: 统一使用了read/write, 后续如果为了减少空间, 可以像驱动那样封装一层 */
typedef struct 
{
    const char* name;
    int (*read)(unsigned int add, void* buff, int len);
    int (*write)(unsigned int add, void* buff, int len);
}device_t;


#define RT_USED                     __attribute__((used))
#define SECTION(x)                  __attribute__((section(x)))
           
#define DEVICE_CREAT(obj, level)       static RT_USED const device_t obj SECTION(".device." level)
#define CREAT_DEVICE(obj)           DEVICE_CREAT(obj, "0.1") //创建设备

#define DRIVER_CREAT(obj, level)       static RT_USED const driver_t obj SECTION(".driver." level)
#define CREAT_DRIVER(obj)           DRIVER_CREAT(obj, "0.1") //创建驱动

const device_t* find_device(const char* name);
const driver_t* find_driver(const char* name);

#endif
