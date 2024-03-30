#include "button.h"

/* 获取定时器时间的回调 */
static SysTime GetSysTime;
void BtnInit(SysTime cb)
{
    GetSysTime = cb;
}

/* 功能: 获取按键状态, 目前这种只适合释放后触发事件 
 * 参数: Btn: 按键对象, cur_sta: 当前按键IO按下释放的状态(如果按下是高电平,需要将该值取反)
 */
BtnEvt_t GetBtnEvt(Btn_t *Btn, BtnSta_t cur_sta)
{
#define SYSTIME_MAX     0xFFFFFFFF
#define _100MS           20
#define _200MS          200
#define _1000MS         1000

    unsigned int press_time, systime;
    BtnEvt_t btn_sta = PRESS_NONE;

    /* 按键首次按下 */
    if (cur_sta == KEY_PRESS && Btn->PreSta == KEY_RELEASE) {
        Btn->StartTime = GetSysTime();
        Btn->PreSta = KEY_PRESS;
    }

    /* 释放按键后触发事件 */
    if (cur_sta == KEY_RELEASE) {
        systime = GetSysTime();
        if(Btn->PreSta == KEY_RELEASE) {
            Btn->StartTime = systime;
        }
        press_time = systime >= Btn->StartTime ? systime - Btn->StartTime : SYSTIME_MAX - Btn->StartTime + systime;

        if (_100MS < press_time && press_time < _200MS) {
            btn_sta = PRESS_SHORT;
        } else if (_200MS <= press_time && press_time < _1000MS) {
            btn_sta = PRESS_200MS;
        } else if (_1000MS <= press_time) {
            btn_sta = PRESS_1000MS;
        }

#if SUPPORT_MULTI_CLICK
        if(btn_sta > PRESS_SHORT) {     /* 第一次短按,接着长按,则认为是长按 */
            Btn->ClickCount = 0;
        }
        if(btn_sta == PRESS_SHORT)
        {
            Btn->ClickCount++;          /* 记录短按次数 */
            Btn->ReleaseTime = systime; /* 短按后的第一次释放时间 */
            btn_sta = PRESS_NONE;   /* 短按后,要先等待以下看是否有多击事件 */
        }

        if (Btn->ClickCount) {
            unsigned int wait_time = (systime >= Btn->ReleaseTime) ? (systime - Btn->ReleaseTime) : (SYSTIME_MAX - Btn->ReleaseTime + systime);
            if(wait_time > _200MS) {
                btn_sta = PRESS_SHORT;  /* 等待200ms没有, 则上报当前按下为单击 */
                Btn->ClickCount = 0;
            } else {
                /* 多击只能多选一, 否则冲突 */
               if(Btn->ClickCount == 2) {
                   btn_sta = PRESS_2TIMES;
                   Btn->ClickCount = 0;
               }

                // if (Btn->ClickCount == 3) {
                //     btn_sta = PRESS_3TIMES;
                //     Btn->ClickCount = 0;
                // }
            }
        }
#endif
        Btn->StartTime = systime;
        Btn->PreSta = KEY_RELEASE;
    }

    return btn_sta;
}
