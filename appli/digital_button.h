/*
 * digital_button.h
 *
 *  Created on: 20 dec. 2023
 *      Author: Arnaud Morillon
 */
#include "macro_types.h"

typedef struct{
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
    uint16_t fontWidth;
    uint16_t fontHeight;
    uint8_t bigger;
    char text[10];
}button_t;

void button_init(void);
button_t get_pauseMenuButton(uint8_t index);
void draw_menuButton(void);
void draw_pauseMenuButtons(void);
