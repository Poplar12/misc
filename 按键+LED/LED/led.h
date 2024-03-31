#ifndef LED_H
#define LED_H

typedef enum
{
    NONE = 0,
    OFF,
    ON,
    ON_3Times,
    BREATH,
    LED_MODE_MAX,
}ledsta_t;

void analyze_led_state(void);
void send_led_state(ledsta_t sta);

#endif
