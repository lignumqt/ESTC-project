#include <stdbool.h>
#include <stdint.h>
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
    static const uint8_t leds_seq[] = led_seq_count_blink;

    /* Configure board. */
    board_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS);

    /* Toggle LEDs. */
    while (true)
    {
        for (int j = 0; j < LEDS_NUMBER; j++)
        {
            while (nrf_gpio_pin_read(BUTTON_1) == BTN_RELEASED) {}

            for (int i = 0; i < leds_seq[j]; i++)
            {
                led_blink_by_idx(j);

                while (nrf_gpio_pin_read(BUTTON_1) == BTN_RELEASED) {}
            }
        }
    }
}

/**
 *@}
 **/
