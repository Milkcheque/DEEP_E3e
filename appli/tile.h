/*
 * tile.h
 *
 *  Created on: 1 d�c. 2023
 *      Author: Arnaud Morillon
 */
#include "macro_types.h"

#ifndef TILE_H_
#define TILE_H_

typedef struct{
    uint16_t width;
    uint16_t height;
    uint16_t pos[2];
    uint16_t prev_pos[2];
}tile_t;

tile_t * getTiles(void);
int16_t getIndexTile(void);
void createTile(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void drawGround(uint16_t offset[2]);

#endif /* TILE_H_ */
