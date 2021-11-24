#include <stdbool.h>
#include <stdint.h>

#include "gpio.h"
#include "pca10059.h"
#include "boards.h"
#include "button.h"
#include "led.h"

void board_init(uint32_t init_flags)
{
    #if LEDS_NUMBER > 0
    if (init_flags & BSP_INIT_LEDS)
    {
        led_board_init();
    }
    #endif //LEDS_NUMBER > 0

    #if BUTTONS_NUMBER > 0
    if (init_flags & BSP_INIT_BUTTONS)
    {
        button_board_init();
    }
    #endif //BUTTONS_NUMBER > 0
}
