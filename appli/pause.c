/*
 * pause.c
 *
 *  Created on: 21 dec. 2023
 *      Author: Arnaud Morillon
 */

#include "pause.h"
#include "macro_types.h"
#include "stm32f1_xpt2046.h"


bool checkScreenTouch(void)
{
    uint8_t x, y;
    if(XPT2046_getCoordinates(&x, &y, XPT2046_COORDINATE_SCREEN_RELATIVE))
        return true;
    else
        return false;
}
