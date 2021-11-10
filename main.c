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
    /* Configure board. */
    board_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS);

    /* Toggle LEDs. */
    while (true)
    {
        if_button_press_check(led_toogle_by_sequence);
    }
}

/**
 *@}
 **/
