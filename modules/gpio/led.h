#ifndef LED_H
#define LED_H

#include "gpio.h"
#include "app_timer.h"

// LED definitions for PCA10059
// Each LED color is considered a separate LED
#define LEDS_NUMBER    4

#define LED1_G         NRF_GPIO_PIN_MAP(0,6)
#define LED2_R         NRF_GPIO_PIN_MAP(0,8)
#define LED2_G         NRF_GPIO_PIN_MAP(1,9)
#define LED2_B         NRF_GPIO_PIN_MAP(0,12)

#define LED_1          LED1_G
#define LED_2          LED2_R
#define LED_3          LED2_G
#define LED_4          LED2_B

#define led_seq_count_blink {6, 6, 0, 8}

#define LEDS_ACTIVE_STATE 0
#define LEDS_INACTIVE_STATE 1

#define LEDS_TIMEOUT_TOGGLE APP_TIMER_TICKS(500)

#define LEDS_LIST { LED_1, LED_2, LED_3, LED_4 }

#if LEDS_NUMBER > 0
static const uint8_t m_board_led_list[LEDS_NUMBER] = LEDS_LIST;
#endif //LEDS_NUMBER

static const struct {
    char led_string[255];
} leds_idx_to_string_t[] = {
    {"LED1_G"},
    {"LED2_R"},
    {"LED2_G"},
    {"LED2_B"},
};

void led_toogle_by_idx(uint32_t led_idx);
void led_blink_by_idx(uint32_t led_idx);
void led_on_by_idx(uint32_t led_idx);
void led_off_by_idx(uint32_t led_idx);
void board_leds_init(void);
void led_toogle_by_sequence(void *tmr_id);
bool smooth_flashing_led(bool brighter, int led_idx);
void led_toogle_smooth_by_seq();
void init_systick_timer();

#endif //LED_H