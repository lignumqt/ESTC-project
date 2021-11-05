
#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"
#include "pca10059.h"
#include "sdk_config.h"
#include "gpio.h"
#include "led.h"
#include "button.h"

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    /* Configure board. */
    board_init(BSP_INIT_LEDS);

    /* Toggle LEDs. */
    while (true)
    {
        for (int i = 0; i < LEDS_NUMBER; i++)
        {
            switch (i)
            {
            case LED_IDX_0:
                for (int j = 0; j < COUNT_BLINK_FIRST_LED; j++)
                {
                    led_invert(LED_IDX_0);
                    nrf_delay_ms(100);
                }
                break;
            case LED_IDX_1:
                for (int j = 0; j < COUNT_BLINK_SECOND_LED; j++)
                {
                    led_invert(LED_IDX_1);
                    nrf_delay_ms(500);
                }
                break;
            case LED_IDX_2:
                for (int j = 0; j < COUNT_BLINK_THIRD_LED; j++)
                {
                    led_invert(LED_IDX_2);
                    nrf_delay_ms(500);
                }
                break;
            case LED_IDX_3:
                for (int j = 0; j < COUNT_BLINK_FOURTH_LED; j++)
                {
                    led_invert(LED_IDX_3);
                    nrf_delay_ms(500);
                }
                break;
            }
        }
    }
}

/**
 *@}
 **/
