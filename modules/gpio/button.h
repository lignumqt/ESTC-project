#ifndef BUTTON_H
#define BUTTON_H

#include "pca10059.h"

#if BUTTONS_NUMBER > 0
static const uint8_t m_board_btn_list[BUTTONS_NUMBER] = BUTTONS_LIST;
#endif //BUTTONS_NUMBER

void board_buttons_init(void);

#endif //BUTTON_H