#include <stdbool.h>
#include <stdint.h>
#include "led.h"
#include "gpio.h"

static nrfx_systick_state_t t_on = { 0 };
static nrfx_systick_state_t t_off = { 0 };

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

void led_toogle_by_sequence(void *tmr_id)
{
    static uint16_t led_last = 0;
    static uint16_t led_last_position = 0;
    static const uint8_t leds_seq[] = led_seq_count_blink;

    if (!led_can_be_toogle)
        return;

    if (led_last_position >= leds_seq[led_last])
    {
        led_last_position = 0;
        led_last++;

        if (led_last >= LEDS_NUMBER)
            led_last = 0;

        return;
    }

    led_on_by_idx(led_last);
    NRF_LOG_INFO("Led '%s' turn on", leds_idx_to_string_t[led_last].led_string);

    led_can_be_toogle = 0;
    app_timer_start(tmr_id, LEDS_TIMEOUT_TOGGLE, (void *)&led_last);
    NRF_LOG_INFO("Timer start");

    led_last_position++;

    return;
}

void led_toogle_smooth_by_seq()
{
    static bool brighter = true;
    static bool old_brighter = true;
    static uint16_t led_last = 0;
    static uint16_t led_last_position = 0;
    static const uint8_t leds_seq[] = led_seq_count_blink;

    if (led_last_position >= leds_seq[led_last] * 2)
    {
        led_last_position = 0;
        led_off_by_idx(led_last);
        led_last++;

        if (led_last >= LEDS_NUMBER)
            led_last = 0;

        return;
    }

    brighter = smooth_flashing_led(brighter, led_last);

    if (old_brighter != brighter)
    {
        old_brighter = brighter;
        led_last_position++;
    }
}

bool smooth_flashing_led(bool brighter, int led_idx)
{
    static int start = 0;
    static int stop = TIMEOUT_PWM;
    static int get_t_off = 0;
    static int led_state = 0;

    if (nrfx_systick_test(&t_on, start))
    {
        if (!led_state)
        {
            led_off_by_idx(led_idx);
            led_state = 1;
        }

        if (!get_t_off)
        {
            nrfx_systick_get(&t_off);
            get_t_off = 1;
        }

        if (nrfx_systick_test(&t_off, stop))
        {
            led_on_by_idx(led_idx);
            led_state = 0;
            nrfx_systick_get(&t_on);

            if (brighter)
            {
                start++;
                stop--;
            }
            else
            {
                start--;
                stop++;
            }

            if (((start == 0) && (stop == TIMEOUT_PWM) && !brighter) ||
                ((start == TIMEOUT_PWM) && (stop == 0) && brighter))
            {
                brighter = !brighter;
            }

            get_t_off = 0;
        }
    }

    return brighter;
}

void init_systick_timer()
{
    nrfx_systick_init();
    nrfx_systick_get(&t_on);
}