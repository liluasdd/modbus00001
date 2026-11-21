
#include "led.h"
#include "main.h"
u8 led_num = 0;
void Led_scan(void)
{
    static uint8_t led_state_num = 0;

    if (led_state_num == 0)
    {
        led_state_num = 1;
        if (led_num)
            Led1_on; // À¶µÆ
        else
            Led2_on; // »ÆµÆ
    }
    else
    {
        led_state_num = 0;
        // if (led_num)
        Led1_off;
        // else
        Led2_off;
    }
}
