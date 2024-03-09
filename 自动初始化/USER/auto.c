#include "auto.h"
#include "string.h"
#include "stdio.h"

/* �趨�豸���λ�� */
DEVICE_CREAT(device_start, "0");
DEVICE_CREAT(device_end, "1");
/* ͨ�����ֲ����豸 */
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

/* �趨�������λ�� */
DRIVER_CREAT(driver_start, "0");
DRIVER_CREAT(driver_end, "1");
/* ͨ�����ֲ������� */
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

/* δ�����������һ�������ʼ������, ���߸ɴ಻����, ֱ���ں�����ͷ�ӹؼ���, �����Զ���ʼ�� */
