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


const tile_t level1[15] = {
        {0, 0, {40, 180}, STARTING_POINT},
        {13, 13, {240, 30}, ENDING_POINT},
        {88, 30, {40, 232}, OBSTACLE},
        {58, 30, {70, 225}, OBSTACLE},
        {58, 30, {80, 218}, OBSTACLE},
        {60, 20, {90, 210}, OBSTACLE},
        {33, 24, {200, 187}, OBSTACLE},
        {43, 25, {277, 147}, OBSTACLE},
        {34, 27, {173, 117}, OBSTACLE},
        {14, 30, {102, 114}, OBSTACLE},
        {14, 30, {43, 99}, OBSTACLE},
        {45, 22, {0, 0}, OBSTACLE},
        {205 , 20, {115, 50}, OBSTACLE},
        {205 , 20, {115, 50}, OBSTACLE},
        {46 , 68, {274, 0}, OBSTACLE},
};

const tile_t level2[15] = {
        {0, 0, {46, 0}, STARTING_POINT},
        {13, 13, {305, 150}, ENDING_POINT},
        {45, 50, {0, 0}, OBSTACLE},
        {75, 20, {0, 43}, OBSTACLE},
        {59, 25, {85, 110}, OBSTACLE},
        {28, 30, {0, 180}, OBSTACLE},
        {18, 184, {133, 0}, OBSTACLE},
        {25, 23, {135, 58}, OBSTACLE},
        {24, 23, {135, 161}, OBSTACLE},
        {50, 35, {120, 230}, OBSTACLE},
        {44, 26, {217, 41}, OBSTACLE},
        {15, 199, {247, 41}, OBSTACLE},
        {28, 23, {232, 110}, OBSTACLE},
        {25, 30, {295, 114}, OBSTACLE},
        {5, 47, {315, 193}, OBSTACLE},
};

static uint8_t index_currentLevel = 0;
static uint8_t level_size = 15;

static const tile_t * levels[2];

settings_t settings;

/*
 * @brief   getter pour les paramètres de la map
 * @retval 	settings_t
 */
settings_t * getMapSettings(void)
{
    return &settings;
}

void resetLevel(void)
{
    index_currentLevel = 0;
}

uint8_t getNbLevels(void)
{
    return 2;
}

uint8_t getIndexLevel(void)  
{
    return index_currentLevel;

}

/*
 * @brief 	Getter for the tiles
 * @retval 	tile_t
 */
tile_t * getTiles(void)
{
    return levels[index_currentLevel];
}

/*
 * @brief 	Getter du nombre de tuiles
 * @retval 	uint8_t
 */
uint8_t * getLevelSize(void)
{
    return &level_size;
}

/*
 * @brief 	Passe au niveau suivant
 */
void nextLevel(void)
{
    index_currentLevel++;
}

/*
 * @brief 	Initialize the map
 */
void initMap(void){
    settings.width = 320;
    settings.height = 240;

	levels[0] = &level1;
	levels[1] = &level2;
    
    tile_t element;
    for(uint8_t i=0; i<level_size; i++){
    	element = levels[index_currentLevel][i];
        if(element.role == STARTING_POINT)
            setPosPlayer(element.pos[0], element.pos[1]);
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
    for(uint16_t i=0; i<level_size; i++){
        tile_t tile = levels[index_currentLevel][i];
        if(tile.role == OBSTACLE)
            ILI9341_DrawFilledRectangle(tile.pos[0], tile.pos[1], tile.pos[0]+tile.width, tile.pos[1]+tile.height, ILI9341_COLOR_BLACK);

        if(tile.role == ENDING_POINT)
        {
            ILI9341_DrawFilledRectangle(tile.pos[0], tile.pos[1], tile.pos[0]+tile.width, tile.pos[1]+tile.height, ILI9341_COLOR_BLUE);
            ILI9341_DrawFilledRectangle(tile.pos[0]+3, tile.pos[1]+3, tile.pos[0]+tile.width-4, tile.pos[1]+tile.height-4, ILI9341_COLOR_BLUE2);
        }
    }
}
