#include <stdbool.h>
#include <stdint.h>
#include "gpio.h"
#include "button.h"
#include "nrf_drv_gpiote.h"
#include "pwm.h"

static bool button_one_click = false;
static bool was_double_click = false;

APP_TIMER_DEF(check_double_click_tmr);
APP_TIMER_DEF(button_double_click_control_pwm_tmr);

bool button_get_was_double_click()
{
    return was_double_click;
}

void button_board_init(void)
{
    uint32_t i;
    for (i = 0; i < BUTTONS_NUMBER; ++i)
    {
        nrf_gpio_cfg_input(m_board_btn_list[i], BUTTON_PULL);
    }
}

void button_if_press_check(button_action_cb callback, void *data)
{
    if (nrf_gpio_pin_read(BUTTON_1) == BTN_PRESSED)
    {
        callback(data);
    }
}

static void button_if_double_click_check_cb(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (!button_one_click)
    {
        button_one_click = true;
        app_timer_start(check_double_click_tmr, CHECK_DOUBLE_CLICK_TIMEOUT, NULL);
    }
    else if (button_one_click)
    {
        was_double_click = !was_double_click;
        button_one_click = false;
        app_timer_start(button_double_click_control_pwm_tmr, APP_TIMER_TICKS(200), NULL);
        app_timer_stop(check_double_click_tmr);
    }
}

void button_gpio_config_init(void)
{
    ret_code_t err_code;

    err_code = nrfx_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_config_t button_config = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    button_config.pull = NRF_GPIO_PIN_PULLUP;

    err_code = nrfx_gpiote_in_init(BUTTON_1, &button_config, button_if_double_click_check_cb);
    APP_ERROR_CHECK(err_code);

    nrfx_gpiote_in_event_enable(BUTTON_1, true);
}

static void button_single_press_reset(void *p_context)
{
    button_one_click = false;
}

static void button_double_click_control_pwm(void *p_context)
{
    pwm_main_control_phase_choice();
}

void button_timer_init()
{
    app_timer_create(&button_double_click_control_pwm_tmr, APP_TIMER_MODE_SINGLE_SHOT, button_double_click_control_pwm);
    app_timer_create(&check_double_click_tmr, APP_TIMER_MODE_SINGLE_SHOT, button_single_press_reset);
}