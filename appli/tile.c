/*
 * tile.c
 *
 *  Created on: 1 d�c. 2023
 *      Author: Arnaud Morillon
 */
#include "tile.h"
#include "stm32f1_ili9341.h"

static uint8_t size = 200;
static tile_t tiles[200];
static int16_t indexTile = -1;


/*
 * @brief 	Getter pour l'index de la tuile
 * @retval 	int16_t indice de la tuile
 */
int16_t getIndexTile(void){
    return indexTile;
}

void initTiles(void){
    indexTile = -1;
}

/*
 * @brief 	Créé une tuile
 * @param 	x: position en x de la tuile
 * @param 	y: position en y de la tuile
 * @param 	width: largeur de la tuile
 * @param 	height: hauteur de la tuile
 * @param 	role: role de la tuile
 */
void createTile(uint16_t pos, uint16_t width, uint16_t height, role_t role){
    tiles[++indexTile] = (tile_t){width, height, pos, role};
}
