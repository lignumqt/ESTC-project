#ifndef GPIO_H
#define GPIO_H

#include "nordic_common.h"
#include "boards.h"

#include "pca10059.h"
#include "sdk_config.h"
#include "led.h"
#include "button.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_log_backend_usb.h"

#include "app_usbd.h"
#include "app_usbd_serial_num.h"

#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "app_timer.h"
#include "nrfx_systick.h"
#include "app_util_platform.h"

#define TIMEOUT_PWM 1000

/** @brief Macro for mapping port and pin numbers to values understandable for nrf_gpio functions. */
#define NRF_GPIO_PIN_MAP(port, pin) (((port) << 5) | ((pin) & 0x1F))

void board_init(uint32_t init_flags);
extern int led_can_be_toogle;

#endif //GPIO_H