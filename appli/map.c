/*
 * virtual_map.c
 *
 *  Created on: 30 nov. 2023
 *      Author: Arnaud Morillon
 */

#include "map.h"
#include "macro_types.h"
#include "player.h"
#include "stm32f1_ili9341.h"

settings_t settings;

const tile_t level1[15] = {
        {0, 0, {40, 180}, STARTING_POINT},
        {0, 0, {240, 30}, ENDING_POINT},
        {88, 30, {40, 232}, OBSTACLE},
        {58, 30, {70, 225}, OBSTACLE},
        {58, 30, {80, 218}, OBSTACLE},
        {60, 20, {90, 210}, OBSTACLE},
        {33, 24, {200, 187}, OBSTACLE},
        {43, 25, {277, 147}, OBSTACLE},
        {34, 27, {173, 117}, OBSTACLE},
        {14, 30, {102, 114}, OBSTACLE},
        {14, 30, {43, 99}, OBSTACLE},
        {38, 22, {0, 0}, OBSTACLE},
        {205 , 20, {115, 50}, OBSTACLE},
        {205 , 20, {115, 50}, OBSTACLE},
        {46 , 68, {274, 0}, OBSTACLE},
};

static const tile_t * current_level = level1;
static uint8_t current_level_size = 15;

/*
 * @brief   getter pour les paramètres de la map
 * @retval 	settings_t
 */
settings_t * getMapSettings(void)
{
    return &settings;
}

/*
 * @brief 	Getter for the tiles
 * @retval 	tile_t
 */
tile_t * getTiles(void)
{
    return current_level;
}

/*
 * @brief 	Getter du nombre de tuiles
 * @retval 	uint8_t
 */
uint8_t * getLevelSize(void)
{
    return &current_level_size;
}

/*
 * @brief 	Initialize the map
 */
void initMap(void){
    settings.width = 320;
    settings.height = 240;
    tile_t element;
    for(uint8_t i=0; i<current_level_size; i++){
    	element = current_level[i];
        /*
        if(element.role == OBSTACLE){
            createTile(element.pos, element.width, element.height, element.role);
        }
        else*/ if(element.role == STARTING_POINT){
            setPosPlayer(element.pos[0], element.pos[1]);
        }
    }

    /*
    for(uint16_t y=0; y<settings.height/10; y++){
        for(uint16_t x=0; x<settings.width/10; x++){
            if(map[y][x] == 1){
                //createTile(x*10,y*10,10,10);
            }
            else if(map[y][x] == 5){
                setPosPlayer(x*10, y*10);
            }
        }
    }
    createTile(40,200,75,15);
    createTile(115,180,30,45);
    createTile(145,155,120,15);
    createTile(90,155,25,15);
    */
}


/*
 * @brief 	Affiche toutes les tuiles du niveaux
 */
void drawMap(void){
    for(uint16_t i=0; i<current_level_size; i++){
        tile_t tile = current_level[i];
        if(tile.role == OBSTACLE)
            ILI9341_DrawFilledRectangle(tile.pos[0], tile.pos[1], tile.pos[0]+tile.width, tile.pos[1]+tile.height, ILI9341_COLOR_BLACK);
    }
}
