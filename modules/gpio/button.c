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

void if_button_press_check(button_action_cb callback, void *data)
{
    if (nrf_gpio_pin_read(BUTTON_1) == BTN_PRESSED)
    {
        callback(data);
    }
}
