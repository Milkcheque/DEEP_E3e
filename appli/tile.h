/*
 * tile.h
 *
 *  Created on: 1 dec. 2023
 *      Author: Arnaud Morillon
 */
#include "macro_types.h"

#ifndef TILE_H_
    #define TILE_H_
#endif /* TILE_H_ */

typedef enum{
    STARTING_POINT,
    ENDING_POINT,
    OBSTACLE,
}role_t;

typedef struct{
    uint16_t width;
    uint16_t height;
    uint16_t pos[2];
    role_t role;
}tile_t;

int16_t getIndexTile(void);
void createTile(uint16_t pos, uint16_t width, uint16_t height, role_t role);
