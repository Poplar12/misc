#ifndef BUTTON_H
#define BUTTON_H
#include "export.h"

#define SUPPORT_MULTI_CLICK         1      /* 支持多击 */

typedef enum
{
    KEY_PRESS = 0,
    KEY_RELEASE,
}BtnSta_t;
typedef enum
{
    PRESS_NONE = 0,
    PRESS_SHORT,
    PRESS_200MS,
    PRESS_1000MS,
#if SUPPORT_MULTI_CLICK
    PRESS_2TIMES,
    PRESS_3TIMES,
#endif
}BtnEvt_t;

/* 通过自动初始化将需要处理的事情注册过来 */
typedef void(*bnt_cb)(BtnEvt_t evt);

#define BNT_EVENT(obj, level)       static RT_USED const bnt_cb obj SECTION(".bnt_event." level)
#define ADD_EVENT(obj)           BNT_EVENT(key_##obj, "0.1") = obj;

typedef unsigned int (*SysTime)();

typedef struct 
{
    BtnSta_t PreSta;            /* 按键上一个状态 */
    unsigned int StartTime;     /* 第一次按下的时间 */
#if SUPPORT_MULTI_CLICK
    unsigned int ReleaseTime;   /* 第一次释放的时间 */
    unsigned char ClickCount;   /* 短按的次数 */
#endif
}Btn_t;

void BtnInit(SysTime cb);
BtnEvt_t GetBtnEvt(Btn_t *Btn, BtnSta_t cur_sta);

#endif
