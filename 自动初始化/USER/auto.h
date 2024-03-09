
#ifndef AUTO_H
#define AUTO_H

/* �����ṹ��: ���ڲ�ͬ������, ��Ҫ�Ľӿں�����ͬ, Ϊ�˽�ʡ�ռ��ٷ���һ�� */
typedef struct
{
    const char* name;
    void* opt;
}driver_t;

/* �豸�ṹ��: ͳһʹ����read/write, �������Ϊ�˼��ٿռ�, ����������������װһ�� */
typedef struct 
{
    const char* name;
    int (*read)(unsigned int add, void* buff, int len);
    int (*write)(unsigned int add, void* buff, int len);
}device_t;


#define RT_USED                     __attribute__((used))
#define SECTION(x)                  __attribute__((section(x)))
           
#define DEVICE_CREAT(obj, level)       static RT_USED const device_t obj SECTION(".device." level)
#define CREAT_DEVICE(obj)           DEVICE_CREAT(obj, "0.1") //�����豸

#define DRIVER_CREAT(obj, level)       static RT_USED const driver_t obj SECTION(".driver." level)
#define CREAT_DRIVER(obj)           DRIVER_CREAT(obj, "0.1") //��������

const device_t* find_device(const char* name);
const driver_t* find_driver(const char* name);

#endif
