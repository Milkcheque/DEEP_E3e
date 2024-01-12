/*
 * virtual_map.h
 *
 *  Created on: 30 nov. 2023
 *      Author: Arnaud Morillon
 */

#include "macro_types.h"
#include "tile.h"

#ifndef VIRTUAL_MAP_H_
#define VIRTUAL_MAP_H_

typedef struct{
    uint16_t width;
    uint16_t height;
}settings_t;

void initMap(void);
tile_t * getTiles(void);
uint8_t * getLevelSize(void);
uint8_t getIndexLevel(void);
uint8_t getNbLevels(void);
void nextLevel(void);
void resetLevel(void);
settings_t * getMapSettings(void);
void drawMap(void);

#endif /* VIRTUAL_MAP_H_ */
