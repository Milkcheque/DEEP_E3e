/*
 * player.c
 *
 *  Created on: 22 nov. 2023
 *      Author: Arnaud Morillon
 */
#include "player.h"
#include "stm32f1_adc.h"
#include "stm32f1_ili9341.h"
#include "virtual_map.h"
#include "tile.h"

#define ADC_JOYSTICK_X_CHANNEL	ADC_1
#define ADC_JOYSTICK_Y_CHANNEL	ADC_0	//sont invers�s intentionnellement pour avoir le d�placement sur le bon axe
#define GPIO_PIN_BP_JUMP		GPIO_PIN_8

void jump(void);
void applyGravity(void);
void checkCollision(void);
bool rightCollision(tile_t tile);
bool leftCollision(tile_t tile);
bool bottomCollision(tile_t tile);
bool topCollision(tile_t tile);

static player_t player;
static physicalStatus_t physStatus;
static playerStatus_e playerStatus;
static cooldown_t cooldown;

/*
 * @brief 	Getter for player attributes
 * @retval 	player_t
 */
player_t * getPlayer(void)
{
	return &player;
}

/*
 * @brief 	Getter for the cooldowns of the player
 * @retval 	cooldown_t
 */
cooldown_t * getCooldown(void)
{
	return &cooldown;
}

/*
 * @brief 	Set the position of the player
 * @param 	x: x position
 * @param 	y: y position
 */
void setPosPlayer(uint16_t x, uint16_t y)
{
	player.pos_x = x;
	player.pos_y = y;
}

/*
 * @brief 	Initialise les attributs du joueurs
 */
void initPlayer(void)
{
	//Init player
	player.prev_pos_x = player.pos_x;
	player.prev_pos_y = player.pos_y;
	player.width = 25;
	player.height = 30;
	player.speed_x = 0;
	player.speed_y = 0;
	player.health = 50;
	//Init hitbox
	player.hitbox_pos[0] = (int16_t)(player.pos_x + 5);
	player.hitbox_pos[1] = (int16_t)(player.pos_y + 10);
	player.hitbox_width = (int8_t)(player.width - 10);
	player.hitbox_height = (int8_t)(player.height - 10);
	//Init physical status
	physStatus.onGround, physStatus.onCeiling, physStatus.onLeft, physStatus.onRight = false;
	//Init cooldowns
	cooldown.hasJumped, cooldown.hasShot = false;
	cooldown.jumpCD = 150;
	cooldown.shootCD =500;
	//Init player status
	playerStatus = IDLE;
}

/*
 * @brief 	Method to update player position
 */
void update_playerMovement(void)
{
 	uint16_t X = (uint16_t)ADC_getValue(ADC_JOYSTICK_X_CHANNEL);
	if(X < 2000)
	{
		player.hitbox_pos[0] -= (int16_t)((4095-X/4095)*5);
	} else if(X > 2120)
	{
		player.hitbox_pos[0] += (int16_t)((4095-X/4095)*5);	//FAUDRAIT PAS METTRE CA DANS joystick.c ?????
	}
	if(!physStatus.onGround && player.hitbox_pos[1] + player.hitbox_height < 240)
	{
		applyGravity();
	}
	else
	{
		player.speed_y = 0;
		player.hitbox_pos[1] = 240 - player.hitbox_height;
	}
	//Jump
	if(!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_BP_JUMP) && !cooldown.hasJumped)
	{
		jump();
	}
	player.hitbox_pos[1] += player.speed_y;
	//Limites
	if(player.hitbox_pos[0] < 0)
		player.hitbox_pos[0] = 0;
	else if(player.hitbox_pos[0] + player.hitbox_width > getMapSettings()->width)
		player.hitbox_pos[0] = getMapSettings()->width - player.hitbox_width;
	if(player.hitbox_pos[1] < 0)
		player.hitbox_pos[1] = 0;
	else if(player.hitbox_pos[1] + player.hitbox_height > getMapSettings()->height)
		player.hitbox_pos[1] = getMapSettings()->height - player.hitbox_height;
} 

/*
 * @brief 	Apply gravity to the player
 */
void applyGravity(void){
	player.speed_y += 3;
}

/*
 * @brief 	Make the player jump when the cooldown is over
 */
void jump(void){
	cooldown.hasJumped = true;
	player.speed_y = -20;
}

/*
 * @brief 	Vérifie si le joueur est en collision avec une tuile
 */
//Pour savoir si collision -> if(horizontalCollision() && verticalCollision()) ou juste faire en fonction des physStatus
void checkCollision(void){
	tile_t * tiles = getTiles();
	physStatus.onGround, physStatus.onCeiling, physStatus.onLeft, physStatus.onRight = false;
	for(uint8_t i=0; i<=getIndexTile(); i++)
	{
		//Collision à droite
		if(rightCollision(tiles[i]) && (bottomCollision(tiles[i]) || topCollision(tiles[i])))
		{
			physStatus.onRight = true;
			player.hitbox_pos[0] = tiles[i].pos[0] - player.width;
		}
		//Collision à gauche
		else if(leftCollision(tiles[i]) && (bottomCollision(tiles[i]) || topCollision(tiles[i])))
		{
			physStatus.onLeft = true;
			player.hitbox_pos[0] = tiles[i].pos[0] + tiles[i].width;
		}
		//Collision en bas
		if(bottomCollision(tiles[i]) && (rightCollision(tiles[i]) || leftCollision(tiles[i])))
		{
			physStatus.onGround = true;
			player.hitbox_pos[1] = tiles[i].pos[1] - player.height;
		}
		//Collision en haut
		if(topCollision(tiles[i]) && (rightCollision(tiles[i]) || leftCollision(tiles[i])))
		{
			physStatus.onCeiling = true;
			player.hitbox_pos[1] = tiles[i].pos[1] + tiles[i].height;
		}
	}
	//player.pos_x = player.hitbox_pos[0] - 5;
	//player.pos_y = player.hitbox_pos[1] - 10;
}

/*
 * @brief 	Vérifie si le joueur est en collision avec une tuile sur sa droite
 */
bool rightCollision(tile_t tile)
{
	if(player.hitbox_pos[0] + player.hitbox_width > tile.pos[0] && player.hitbox_pos[0] < tile.pos[0])
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*
 * @brief 	Vérifie si le joueur est en collision avec une tuile sur sa gauche
 */
bool leftCollision(tile_t tile)
{
	if(player.hitbox_pos[0] + player.hitbox_width > tile.pos[0] + tile.width && player.hitbox_pos[0] < tile.pos[0] + tile.width)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*
 * @brief 	Vérifie si le joueur est en collision avec une tuile en dessous de lui
 */
bool bottomCollision(tile_t tile)
{
	if(player.hitbox_pos[1] + player.hitbox_height > tile.pos[1] && player.hitbox_pos[1] < tile.pos[1])
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*
 * @brief 	Vérifie si le joueur est en collision avec une tuile au dessus de lui
 */
bool topCollision(tile_t tile)
{
	if(player.hitbox_pos[1] + player.hitbox_height > tile.pos[1] + tile.height && player.hitbox_pos[1] < tile.pos[1] + tile.height)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void shoot(void){
	//TODO
}

void death(void){
	//TODO
}

void damaged(uint8_t dmg){
	//TODO
}

/*
 * @brief 	Draw the player
 * @param 	offset: offset of the virtual map
 */
void drawPlayer(){
	/*
	ILI9341_DrawFilledRectangle(player.prev_pos_x, player.prev_pos_y, player.prev_pos_x + player.width, player.prev_pos_y + player.height, ILI9341_COLOR_WHITE);
	ILI9341_putImage(player.pos_x, player.pos_y,25,30,getAnim(RUN),750);
	incrementIndexAnim();
	*/
	//Efface l'ancienne image du joueur
	ILI9341_DrawFilledRectangle(player.prev_pos_x, player.prev_pos_y, player.prev_pos_x + player.width, player.prev_pos_y + player.height, ILI9341_COLOR_WHITE);
	//Affiche la nouvelle
	int16_t posX = player.hitbox_pos[0]-5;
	int16_t posY = player.hitbox_pos[1]-10;
	ILI9341_DrawFilledRectangle(posX, posY, posX + player.width, posY + player.height, ILI9341_COLOR_BLUE);
	ILI9341_DrawFilledRectangle(player.hitbox_pos[0], player.hitbox_pos[1], player.hitbox_pos[0] + player.hitbox_width, player.hitbox_pos[1] + player.hitbox_height, ILI9341_COLOR_RED);
	
	player.prev_pos_x = posX;
	player.prev_pos_y = posY;
}

/*
 * @brief 	Update the player
 */
void updatePlayer(void){
	update_playerMovement();
	checkCollision();
}
