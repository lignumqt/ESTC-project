#ifndef GPIO_H
#define GPIO_H

#include "nrf_gpio.h"

/** @brief Macro for mapping port and pin numbers to values understandable for nrf_gpio functions. */
#define NRF_GPIO_PIN_MAP(port, pin) (((port) << 5) | ((pin) & 0x1F))

void gpio_pin_toggle(uint32_t pin_number);
void board_init(uint32_t init_flags);
void nrf_gpio_cfg_input_write(uint32_t pin_number, nrf_gpio_pin_pull_t pull_config);
void nrf_gpio_cfg_output_write(uint32_t pin_number);
void nrf_gpio_cfg_write(uint32_t pin_number, nrf_gpio_pin_dir_t dir, nrf_gpio_pin_input_t input, nrf_gpio_pin_pull_t  pull, nrf_gpio_pin_drive_t drive, nrf_gpio_pin_sense_t sense);
void __nrf_gpio_pin_write(uint32_t pin_number, uint32_t value);
void __nrf_gpio_pin_clear(uint32_t pin_number);
void __nrf_gpio_port_out_clear(NRF_GPIO_Type * p_reg, uint32_t clr_mask);
void __nrf_gpio_pin_set(uint32_t pin_number);
void __nrf_gpio_port_out_set(NRF_GPIO_Type * p_reg, uint32_t set_mask);
void __gpio_output_voltage_setup(void);
uint32_t __nrf_gpio_pin_read(uint32_t pin_number);
uint32_t __nrf_gpio_port_in_read(NRF_GPIO_Type const * p_reg);

#endif //GPIO_H