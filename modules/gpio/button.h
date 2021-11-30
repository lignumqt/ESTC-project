#ifndef BUTTON_H
#define BUTTON_H

#include "pca10059.h"
#include "app_timer.h"

#if BUTTONS_NUMBER > 0
static const uint8_t m_board_btn_list[BUTTONS_NUMBER] = BUTTONS_LIST;
#endif //BUTTONS_NUMBER

#define BTN_PRESSED     0                       /**< Value of a pressed button. */
#define BTN_RELEASED    1                       /**< Value of a released button. */

#define CHECK_DOUBLE_CLICK_TIMEOUT APP_TIMER_TICKS(300)

typedef void (*button_action_cb)();

void button_board_init(void);
void button_if_press_check(button_action_cb callback, void *data);
void button_gpio_config_init(void);
void button_timer_init();
bool button_get_was_double_click();

#endif //BUTTON_H