#include <stdbool.h>
#include <stdint.h>

#include "gpio.h"
#include "pca10059.h"
#include "boards.h"
#include "ff.h"
#include "button.h"
#include "led.h"

#if defined(BOARD_PCA10059)
/**
 * Function for configuring UICR_REGOUT0 register
 * to set GPIO output voltage to 3.0V.
 */
void __gpio_output_voltage_setup(void)
{
    // Configure UICR_REGOUT0 register only if it is set to default value.
    if ((NRF_UICR->REGOUT0 & UICR_REGOUT0_VOUT_Msk) ==
        (UICR_REGOUT0_VOUT_DEFAULT << UICR_REGOUT0_VOUT_Pos))
    {
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}

        NRF_UICR->REGOUT0 = (NRF_UICR->REGOUT0 & ~((uint32_t)UICR_REGOUT0_VOUT_Msk)) |
                            (UICR_REGOUT0_VOUT_3V0 << UICR_REGOUT0_VOUT_Pos);

        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}

        // System reset is needed to update UICR registers.
        NVIC_SystemReset();
    }
}
#endif

void gpio_pin_toggle(uint32_t pin_number)
{
    NRF_GPIO_Type * reg        = nrf_gpio_pin_port_decode(&pin_number);
    uint32_t        pins_state = reg->OUT;

    reg->OUTSET = (~pins_state & (1UL << pin_number));
    reg->OUTCLR = (pins_state & (1UL << pin_number));
}

void board_init(uint32_t init_flags)
{
    #if defined(BOARDS_WITH_USB_DFU_TRIGGER) && defined(BOARD_PCA10059)
    (void) nrf_dfu_trigger_usb_init();
    #endif

    #if LEDS_NUMBER > 0
    if (init_flags & BSP_INIT_LEDS)
    {
        board_leds_init();
    }
    #endif //LEDS_NUMBER > 0

    #if BUTTONS_NUMBER > 0
    if (init_flags & BSP_INIT_BUTTONS)
    {
        board_buttons_init();
    }
    #endif //BUTTONS_NUMBER > 0
}

void nrf_gpio_cfg_input_write(uint32_t pin_number, nrf_gpio_pin_pull_t pull_config)
{
    nrf_gpio_cfg_write(
        pin_number,
        NRF_GPIO_PIN_DIR_INPUT,
        NRF_GPIO_PIN_INPUT_CONNECT,
        pull_config,
        NRF_GPIO_PIN_S0S1,
        NRF_GPIO_PIN_NOSENSE);
}

void nrf_gpio_cfg_output_write(uint32_t pin_number)
{
    nrf_gpio_cfg_write(
        pin_number,
        NRF_GPIO_PIN_DIR_OUTPUT,
        NRF_GPIO_PIN_INPUT_DISCONNECT,
        NRF_GPIO_PIN_NOPULL,
        NRF_GPIO_PIN_S0S1,
        NRF_GPIO_PIN_NOSENSE);
}

void nrf_gpio_cfg_write(
    uint32_t             pin_number,
    nrf_gpio_pin_dir_t   dir,
    nrf_gpio_pin_input_t input,
    nrf_gpio_pin_pull_t  pull,
    nrf_gpio_pin_drive_t drive,
    nrf_gpio_pin_sense_t sense)
{
    NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);

    reg->PIN_CNF[pin_number] = ((uint32_t)dir << GPIO_PIN_CNF_DIR_Pos)
                               | ((uint32_t)input << GPIO_PIN_CNF_INPUT_Pos)
                               | ((uint32_t)pull << GPIO_PIN_CNF_PULL_Pos)
                               | ((uint32_t)drive << GPIO_PIN_CNF_DRIVE_Pos)
                               | ((uint32_t)sense << GPIO_PIN_CNF_SENSE_Pos);
}

void __nrf_gpio_pin_write(uint32_t pin_number, uint32_t value)
{
    if (value == 0)
    {
        __nrf_gpio_pin_clear(pin_number);
    }
    else
    {
        __nrf_gpio_pin_set(pin_number);
    }
}

void __nrf_gpio_pin_clear(uint32_t pin_number)
{
    NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);

    __nrf_gpio_port_out_clear(reg, 1UL << pin_number);
}

void __nrf_gpio_port_out_clear(NRF_GPIO_Type * p_reg, uint32_t clr_mask)
{
    p_reg->OUTCLR = clr_mask;
}

void __nrf_gpio_pin_set(uint32_t pin_number)
{
    NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);

    __nrf_gpio_port_out_set(reg, 1UL << pin_number);
}

void __nrf_gpio_port_out_set(NRF_GPIO_Type * p_reg, uint32_t set_mask)
{
    p_reg->OUTSET = set_mask;
}