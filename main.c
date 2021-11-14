#include <stdbool.h>
#include <stdint.h>
#include "boards.h"
#include "pca10059.h"
#include "sdk_config.h"
#include "gpio.h"
#include "led.h"
#include "button.h"

#include "nrfx_systick.h"

APP_TIMER_DEF(led_off_tmr);
APP_TIMER_DEF(led_can_be_toogle_tmr);

int led_can_be_toogle = 1;

static void logs_init()
{
    ret_code_t ret = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(ret);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

static void led_can_be_toogle_timer_handler(void *p_context)
{
    ret_code_t err_code = 0;
    led_can_be_toogle = 1;

    err_code = app_timer_stop(led_can_be_toogle_tmr);
    APP_ERROR_CHECK(err_code);
}

static void led_off_timer_handler(void *p_context)
{
    ret_code_t err_code = 0;
    uint16_t *led_idx = (uint16_t *)p_context;

    led_off_by_idx(*led_idx);
    NRF_LOG_INFO("Led '%s' turn off", leds_idx_to_string_t[*led_idx].led_string);

    err_code = app_timer_stop(led_off_tmr);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(led_can_be_toogle_tmr, LEDS_TIMEOUT_TOGGLE, NULL);
    APP_ERROR_CHECK(err_code);
}

int main(void)
{
    logs_init();
    board_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS);
    init_systick_timer();

    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&led_off_tmr, APP_TIMER_MODE_REPEATED, led_off_timer_handler);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&led_can_be_toogle_tmr, APP_TIMER_MODE_REPEATED, led_can_be_toogle_timer_handler);
    APP_ERROR_CHECK(err_code);

    while (true)
    {
        if_button_press_check(led_toogle_smooth_by_seq, NULL);

        LOG_BACKEND_USB_PROCESS();
        NRF_LOG_PROCESS();
    }
}
