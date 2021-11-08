#include <stdbool.h>
#include <stdint.h>
#include "led.h"
#include "gpio.h"

void led_toogle_by_idx(uint32_t led_idx)
{
    ASSERT(led_idx < LEDS_NUMBER);
    nrf_gpio_pin_toggle(m_board_led_list[led_idx]);
}

void led_blink_by_idx(uint32_t led_idx)
{
    ASSERT(led_idx < LEDS_NUMBER);

    for (int i = 0; i < 2; i++)
    {
        nrf_gpio_pin_toggle(m_board_led_list[led_idx]);
        nrf_delay_ms(500);
    }
}

void led_on_by_idx(uint32_t led_idx)
{
    ASSERT(led_idx < LEDS_NUMBER);
    nrf_gpio_pin_write(m_board_led_list[led_idx], LEDS_ACTIVE_STATE);
}

void led_off_by_idx(uint32_t led_idx)
{
    ASSERT(led_idx < LEDS_NUMBER);
    nrf_gpio_pin_write(m_board_led_list[led_idx], LEDS_INACTIVE_STATE);
}

void board_leds_init(void)
{
    uint32_t i;

    for (i = 0; i < LEDS_NUMBER; ++i)
    {
        nrf_gpio_cfg_output(m_board_led_list[i]);
        led_off_by_idx(i);
    }
}