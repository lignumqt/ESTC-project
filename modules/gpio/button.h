#ifndef BUTTON_H
#define BUTTON_H

#include "pca10059.h"

#if BUTTONS_NUMBER > 0
static const uint8_t m_board_btn_list[BUTTONS_NUMBER] = BUTTONS_LIST;
#endif //BUTTONS_NUMBER

#define BTN_PRESSED     0                       /**< Value of a pressed button. */
#define BTN_RELEASED    1                       /**< Value of a released button. */

typedef void (*button_action_cb)();

void board_buttons_init(void);
void if_button_press_check(button_action_cb callback);

#endif //BUTTON_H