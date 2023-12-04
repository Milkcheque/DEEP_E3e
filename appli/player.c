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
void setPosPlayer(int16_t x, int16_t y)
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
	player.hitbox_pos[1] = (int16_t)(player.pos_y + 5);
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
		player.pos_x -= (int16_t)((4095-X/4095)*5);
	} else if(X > 2120)
	{
		player.pos_x += (int16_t)((4095-X/4095)*5);	//FAUDRAIT PAS METTRE CA DANS joystick.c ?????
	}
	if(!physStatus.onGround && player.pos_y < 180)
	{
		applyGravity();
	} else 
	{
		player.speed_y = 0;
	}
	//Jump
	if(!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_BP_JUMP) && !cooldown.hasJumped)
	{
		jump();
	}
	player.pos_y += player.speed_y;
	//Limites
	if(player.pos_x < 0)
		player.pos_x = 0;
	else if(player.pos_x + player.width > getMapSettings()->width)
		player.pos_x = getMapSettings()->width - player.width;
	if(player.pos_y < 0)
		player.pos_y = 0;
	else if(player.pos_y + player.height > getMapSettings()->height)
		player.pos_y = getMapSettings()->height - player.height;
	//Set the offset of the virtual map
	int16_t offsetX = player.pos_x - 320/2 + player.width/2;
	int16_t offsetY = player.pos_y - 240/2 + player.height/2;
	if(offsetX < 20)
		offsetX = 20;
	else if(offsetX > getMapSettings()->width - 20)
		offsetX = getMapSettings()->width - 20;
	if(offsetY < 20)
		offsetY = 20;
	else if(offsetY > getMapSettings()->height -20)
		offsetY = getMapSettings()->height - 20;
	setOffsetDisplay((uint16_t)offsetX, (uint16_t)offsetY);
}

/*
 * @brief 	Apply gravity to the player
 */
void applyGravity(void){
	player.speed_y += 4;
}

/*
 * @brief 	Make the player jump when the cooldown is over
 */
void jump(void){
	cooldown.hasJumped = true;
	player.speed_y = -20;
}

//Pour savoir si collision -> if(horizontalCollision() && verticalCollision()) ou juste faire en fonction des physStatus
void horizontalCollision(void){
	tile_t * tiles = getTiles();
	for(uint8_t i=0; i<getIndexTile(); i++){
		if(player.hitbox_pos[0] + player.hitbox_width > tiles[i].pos[0] && player.hitbox_pos[0] < tiles[i].pos[0]){
			physStatus.onRight = true;
		}
		if(player.hitbox_pos[0] + player.hitbox_width > tiles[i].pos[0] + tiles[i].width && player.hitbox_pos[0] < tiles[i].pos[0] + tiles[i].width){
			physStatus.onLeft = true;
		}
	}
}

void verticalCollision(void){
	//TODO
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
void drawPlayer(uint16_t offset[2]){
	/*
	ILI9341_DrawFilledRectangle(player.prev_pos_x, player.prev_pos_y, player.prev_pos_x + player.width, player.prev_pos_y + player.height, ILI9341_COLOR_WHITE);
	ILI9341_putImage(player.pos_x, player.pos_y,25,30,getAnim(RUN),750);
	incrementIndexAnim();
	*/
	//Efface l'ancienne image du joueur
	ILI9341_DrawFilledRectangle(player.prev_pos_x + offset[0], player.prev_pos_y + offset[1], player.prev_pos_x + offset[0] + player.width, player.prev_pos_y + offset[1] + player.height, ILI9341_COLOR_WHITE);
	//Affiche la nouvelle
	uint16_t posX = player.pos_x + offset[0];
	uint16_t posY = player.pos_y + offset[1];
	ILI9341_DrawFilledRectangle(posX, posY, posX + player.width, posY + player.height, ILI9341_COLOR_BLUE);
	
	player.prev_pos_x = player.pos_x;
	player.prev_pos_y = player.pos_y;
}

void updatePlayer(void){
	update_playerMovement();
	//playerCollision();
}
