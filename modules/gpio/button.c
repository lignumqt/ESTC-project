#include <stdbool.h>
#include <stdint.h>
#include "gpio.h"
#include "button.h"

void board_buttons_init(void)
{
    uint32_t i;
    for (i = 0; i < BUTTONS_NUMBER; ++i)
    {
        nrf_gpio_cfg_input(m_board_btn_list[i], BUTTON_PULL);
    }
}