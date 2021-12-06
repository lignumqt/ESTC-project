#include <stdbool.h>
#include <stdint.h>
#include "boards.h"
#include "pca10059.h"
#include "sdk_config.h"
#include "gpio.h"
#include "led.h"
#include "button.h"
#include "pwm.h"

#include "nrfx_systick.h"

static void logs_init()
{
    ret_code_t ret = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(ret);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

static void init_all_timers()
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    led_init_timer();
    led_init_systick_timer();
    button_timer_init();
}

int main(void)
{
    logs_init();
    board_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS);
    init_all_timers();
    button_gpio_config_init();
    //for first turn off
    pwm_led_collor();

    while (true)
    {
        LOG_BACKEND_USB_PROCESS();
        NRF_LOG_PROCESS();
    }
}
