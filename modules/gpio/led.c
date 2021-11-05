#include <stdbool.h>
#include <stdint.h>
#include "led.h"
#include "gpio.h"

void led_invert(uint32_t led_idx)
{
    ASSERT(led_idx < LEDS_NUMBER);
    gpio_pin_toggle(m_board_led_list[led_idx]);
}

void board_leds_init(void)
{
    #if defined(BOARD_PCA10059)
    // If nRF52 USB Dongle is powered from USB (high voltage mode),
    // GPIO output voltage is set to 1.8 V by default, which is not
    // enough to turn on green and blue LEDs. Therefore, GPIO voltage
    // needs to be increased to 3.0 V by configuring the UICR register.
    if (NRF_POWER->MAINREGSTATUS &
       (POWER_MAINREGSTATUS_MAINREGSTATUS_High << POWER_MAINREGSTATUS_MAINREGSTATUS_Pos))
    {
        __gpio_output_voltage_setup();
    }
    #endif

    uint32_t i;
    for (i = 0; i < LEDS_NUMBER; ++i)
    {
        nrf_gpio_cfg_output_write(m_board_led_list[i]);
    }
    bsp_board_all_led_off();
}

void board_led_off_by_idx(uint32_t led_idx)
{
    ASSERT(led_idx < LEDS_NUMBER);
    __nrf_gpio_pin_write(m_board_led_list[led_idx], LEDS_ACTIVE_STATE ? 0 : 1);
}

void bsp_board_all_led_off(void)
{
    uint32_t i;
    for (i = 0; i < LEDS_NUMBER; ++i)
    {
        board_led_off_by_idx(i);
    }
}