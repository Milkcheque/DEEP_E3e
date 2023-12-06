/*
 * tile.c
 *
 *  Created on: 1 d�c. 2023
 *      Author: Arnaud Morillon
 */
#include "tile.h"
#include "stm32f1_ili9341.h"

static tile_t tiles[200];
static int16_t indexTile = -1;

/*
 * @brief 	Getter for the tiles
 * @retval 	tile_t
 */
tile_t * getTiles(void){
    return &tiles;
}

/*
 * @brief 	Getter for the index of the tiles
 * @retval 	int16_t
 */
int16_t getIndexTile(void){
    return indexTile;
}

/*
 * @brief 	Create a tile
 * @param 	x: x position of the tile
 * @param 	y: y position of the tile
 * @param 	width: width of the tile
 * @param 	height: height of the tile
 */
void createTile(uint16_t x, uint16_t y, uint16_t width, uint16_t height){
    tiles[++indexTile] = (tile_t){width, height, {x, y}, {x, y}};
}

/*
 * @brief 	Draw a tile
 * @param 	x: x position of the tile
 * @param 	y: y position of the tile
 * @param 	width: width of the tile
 * @param 	height: height of the tile
 */
void drawGround(void){
    for(uint16_t i=0; i<=indexTile; i++){
        //Efface l'ancienne image de la tuile
		//ILI9341_DrawFilledRectangle(tiles[i].prev_pos[0], tiles[i].prev_pos[1], tiles[i].prev_pos[0]+tiles[i].width, tiles[i].prev_pos[1]+tiles[i].height, ILI9341_COLOR_WHITE);
		//Affiche la nouvelle
        ILI9341_DrawFilledRectangle(tiles[i].pos[0], tiles[i].pos[1], tiles[i].pos[0]+tiles[i].width, tiles[i].pos[1]+tiles[i].height, ILI9341_COLOR_BLACK);
    }
}
