#ifndef GPIO_H
#define GPIO_H

#include "nrf_gpio.h"
#include "nrf_delay.h"

/** @brief Macro for mapping port and pin numbers to values understandable for nrf_gpio functions. */
#define NRF_GPIO_PIN_MAP(port, pin) (((port) << 5) | ((pin) & 0x1F))

void board_init(uint32_t init_flags);

#endif //GPIO_H