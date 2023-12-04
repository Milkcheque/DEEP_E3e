/*
 * virtual_map.h
 *
 *  Created on: 30 nov. 2023
 *      Author: Arnaud Morillon
 */

#include "macro_types.h"

#ifndef VIRTUAL_MAP_H_
#define VIRTUAL_MAP_H_

//ILI9341_WIDTH = 240 dispo dans ili9341.h ATTENTION WIDTH ET HEIGHT SONT INVERSES
//ILI9341_HEIGHT = 320

typedef struct{
    uint16_t width;
    uint16_t height;
}settings_t;

void initMap(void);
uint16_t * getOffsetDisplay(void);
void setOffsetDisplay(uint16_t x, uint16_t y);
settings_t * getMapSettings(void);

#endif /* VIRTUAL_MAP_H_ */
