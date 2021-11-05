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
    static const uint8_t leds_seq[] = led_seq;
    int last_led = 0;

    /* Configure board. */
    board_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS);

    /* Toggle LEDs. */
    while (true)
    {
        if (__nrf_gpio_pin_read(BUTTON_1) == BTN_PRESSED)
        {
            for (int j = 0; j < 2; j++)
            {
                led_invert(leds_seq[last_led]);
                nrf_delay_ms(500);
            }
            last_led++;

            if (last_led == sizeof(leds_seq)/sizeof(leds_seq[0]))
                last_led = 0;
        }
    }
}

/**
 *@}
 **/
