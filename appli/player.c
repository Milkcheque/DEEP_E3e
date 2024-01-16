/*
 * player.c
 *
 *  Created on: 22 nov. 2023
 *      Author: Arnaud Morillon
 */
 
#include "player.h"
#include "map.h"
#include "main.h"
#include "stm32f1_adc.h"
#include "stm32f1_ili9341.h"
#include "animation.h"

#define ADC_JOYSTICK_X_CHANNEL	ADC_1
#define ADC_JOYSTICK_Y_CHANNEL	ADC_0	//sont inverses intentionnellement pour avoir le deplacement sur le bon axe
#define GPIO_PIN_BP_JUMP		GPIO_PIN_8
#define GPIO_PIN_BP_DASH		GPIO_PIN_9

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
 * @brief 	Getter for the player orientation
 * @retval 	physicalStatus_t
 */
bool getFacingRight(void)
{
	return physStatus.facingRight;
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
 * @brief 	Set the player status for the animation
 * @param 	state: playerStatus_e
 */
void setPlayerStatus(playerStatus_e state)
{
	playerStatus = state;
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
	player.jumpSpeed = 10;
	player.dashSpeed = 22;
	player.health = 50;
	//Init hitbox
	player.hitbox_pos[0] = (int16_t)(player.pos_x + 5);
	player.hitbox_pos[1] = (int16_t)(player.pos_y + 10);
	player.hitbox_width = (int8_t)(player.width - 10);
	player.hitbox_height = (int8_t)(player.height - 5);
	player.hitbox_pos[1] = (int16_t)(player.pos_y + 10);
	player.hitbox_width = (int8_t)(player.width);
	player.hitbox_height = (int8_t)(player.height);
	//Init physical status
	physStatus.onGround, physStatus.onCeiling, physStatus.onLeft, physStatus.onRight = false;
	physStatus.facingRight = true;
	//Init cooldowns
	cooldown.hasJumped, cooldown.jumpAvailable, cooldown.doubleJumpAvailable, cooldown.hasDashed, cooldown.isDashing = false;
	cooldown.jumpCD = 150;
	cooldown.dashCD =800;
	//Init player status
	playerStatus = IDLE;
}

/*
 * @brief 	Method to update player position
 */
void update_playerMovement(void)
{
	// Déplacement horizontal
 	uint16_t X = (uint16_t)ADC_getValue(ADC_JOYSTICK_X_CHANNEL);
	if(X < 2030)
	{
		player.speed_x = (int16_t)((4095-X)*6/4095);
		physStatus.facingRight = true;
	}
	else if(X > 2112)
	{
		player.speed_x = -(int16_t)(X*6/4095);
		physStatus.facingRight = false;
	}
	else
	{
		player.speed_x = 0;
	}

	// Déplacement vertical
	if(!physStatus.onGround)
		applyGravity();
	else
	{
		player.speed_y = 0;
		// Reset des sauts et double sauts
		cooldown.hasJumped = false;
		cooldown.hasDoubleJumped = false;
		cooldown.jumpAvailable = true;
		cooldown.doubleJumpAvailable = false;
	}

	// Jump
	if(!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_BP_JUMP) && (cooldown.jumpAvailable || cooldown.doubleJumpAvailable))
		jump();

	// Dash
	if((!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_BP_DASH) && !cooldown.hasDashed) || cooldown.isDashing)
		dash();

	player.hitbox_pos[0] += player.speed_x;
	player.hitbox_pos[1] += player.speed_y;

	// Limites de l'écran
	if(player.hitbox_pos[0] < 5)
		player.hitbox_pos[0] = 5;
	else if(player.hitbox_pos[0] + player.hitbox_width > getMapSettings()->width-5)
		player.hitbox_pos[0] = getMapSettings()->width -5 - player.hitbox_width;	// -5 pour eviter une deformation de l'animation du joueur
	if(player.hitbox_pos[1] < 0)
		player.hitbox_pos[1] = 0;
} 

/*
 * @brief 	Apply gravity to the player
 */
void applyGravity(void)
{
	player.speed_y += 2;
}

/*
 * @brief 	Make the player jump when the cooldown is over
 */
void jump(void)
{
	if(cooldown.jumpAvailable)
	{
		cooldown.hasJumped = true;
		cooldown.jumpAvailable = false;
	}
	else if(cooldown.doubleJumpAvailable)
	{
		cooldown.hasDoubleJumped = true;
		cooldown.doubleJumpAvailable = false;
	}
	player.speed_y = -player.jumpSpeed;
 }

/*
 * @brief 	Make the player jump when the cooldown is over
 */
void dash(void)
{
	cooldown.isDashing = true;
	if(getFacingRight())
		player.speed_x = player.dashSpeed;
	else
		player.speed_x = -player.dashSpeed;
	player.speed_y = 0;
 }

/*
 * @brief	Fonction de fin de partie
 */
void checkDeath(void)
{
	if(player.hitbox_pos[1] + player.hitbox_height > getMapSettings()->height && !physStatus.onGround)
		set_state(LOADING_DEATH);
}

/*
 * @brief 	Verifie si le joueur est en collision avec une tuile
 */
void checkCollision(void)
{
	tile_t * tiles = getTiles();
	physStatus.onGround = false;
	physStatus.onCeiling = false;
	physStatus.onLeft = false;
	physStatus.onRight = false;
	for(uint8_t i=0; i< *getLevelSize(); i++)
	{
		if(tiles[i].role == OBSTACLE)
		{
			//Regarde s'il n'y a pas collision	=> code pris de https://jeux.developpez.com/tutoriels/theorie-des-collisions/formes-2d-simples/
			if((tiles[i].pos[0] >= player.hitbox_pos[0] + player.hitbox_width)    	// trop à droite
			|| (tiles[i].pos[0] + tiles[i].width <= player.hitbox_pos[0]) 			// trop à gauche
			|| (tiles[i].pos[1] > player.hitbox_pos[1] + player.hitbox_height)		// trop en bas
			|| (tiles[i].pos[1] + tiles[i].height < player.hitbox_pos[1]))  		// trop en haut
			{}
			else
			{
				//Collision bas et haut
				if(bottomCollision(tiles[i]) && topCollision(tiles[i]))
				{
					//Collision bas et haut droite -> cas collé à un mur
					if(rightCollision(tiles[i]))
					{
						physStatus.onRight = true;
						player.hitbox_pos[0] = tiles[i].pos[0] - player.hitbox_width;
					}
					//Collision bas et haut gauche
					else if(leftCollision(tiles[i]))
					{
						physStatus.onLeft = true;
						player.hitbox_pos[0] = tiles[i].pos[0] + tiles[i].width;
					}
					else
					{
						physStatus.onGround = true;
						player.hitbox_pos[1] = tiles[i].pos[1] - player.hitbox_height;
					}
				}
				//Collision bas
				else if(bottomCollision(tiles[i]))
				{
					if(!(player.hitbox_pos[1]+player.hitbox_height == tiles[i].pos[1] 
					&& (player.hitbox_pos[0]+player.hitbox_width == tiles[i].pos[0] || player.hitbox_pos[0] == tiles[i].pos[0]+tiles[i].width)))	//Vérifie que le joueur n'est pas bloqué à un coin de la tuile
					{
						//Collision bas droite -> partie gauche dans le vide
						if(rightCollision(tiles[i]))
						{
							//physStatus.onRight = true;
							physStatus.onGround = true;
							//player.hitbox_pos[0] = tiles[i].pos[0] - player.hitbox_width;
							player.hitbox_pos[1] = tiles[i].pos[1] - player.hitbox_height;
						}
						//collision bas gauche -> partie droite dans le vide
						else if(leftCollision(tiles[i]))
						{
							//physStatus.onLeft = true;
							physStatus.onGround = true;
							//player.hitbox_pos[0] = tiles[i].pos[0] + tiles[i].width;
							player.hitbox_pos[1] = tiles[i].pos[1] - player.hitbox_height;
						}
						else
						{
							physStatus.onGround = true;
							player.hitbox_pos[1] = tiles[i].pos[1] - player.hitbox_height;
						}
					}
				}
				//collision haut
				else if(topCollision(tiles[i]))
				{
					//Collision haut droite
					if(rightCollision(tiles[i]))
					{
						//physStatus.onRight = true;
						physStatus.onCeiling = true;
						//player.hitbox_pos[0] = tiles[i].pos[0] - player.hitbox_width;
						player.hitbox_pos[1] = tiles[i].pos[1] + tiles[i].height;
					}
					//collision haut gauche -> partie droite dans le vide
					else if(leftCollision(tiles[i]))
					{
						//physStatus.onLeft = true;
						physStatus.onCeiling = true;
						//player.hitbox_pos[0] = tiles[i].pos[0] + tiles[i].width;
						player.hitbox_pos[1] = tiles[i].pos[1] + tiles[i].height;
					}
					else
					{
						physStatus.onCeiling = true;
						player.hitbox_pos[1] = tiles[i].pos[1] + tiles[i].height+1;
					}
				}
				//Collision droite
				else if(rightCollision(tiles[i]))
				{
					physStatus.onRight = true;
					player.hitbox_pos[0] = tiles[i].pos[0] - player.hitbox_width;
				}
				//Collision gauche
				else if(leftCollision(tiles[i]))
				{
					physStatus.onLeft = true;
					player.hitbox_pos[0] = tiles[i].pos[0] + tiles[i].width;
				}
			}
		}
	}
}

/*
 * @brief 	Vérifie si le joueur est en collision avec une tuile sur sa droite
 */
bool rightCollision(tile_t tile)
{
	if(player.hitbox_pos[0] + player.hitbox_width >= tile.pos[0] && player.hitbox_pos[0] <= tile.pos[0])
		return true;
	else
		return false;
}

/*
 * @brief 	Vérifie si le joueur est en collision avec une tuile sur sa gauche
 */
bool leftCollision(tile_t tile)
{
	if(player.hitbox_pos[0] + player.hitbox_width >= tile.pos[0] + tile.width && player.hitbox_pos[0] <= tile.pos[0] + tile.width)
		return true;
	else
		return false;
}

/*
 * @brief 	Vérifie si le joueur est en collision avec une tuile en dessous de lui
 */
bool bottomCollision(tile_t tile)
{
	if(player.hitbox_pos[1] + player.hitbox_height >= tile.pos[1] && player.hitbox_pos[1] <= tile.pos[1])
		return true;
	else
		return false;
}

/*
 * @brief 	Vérifie si le joueur est en collision avec une tuile au dessus de lui
 */
bool topCollision(tile_t tile)
{
	if(player.hitbox_pos[1] + player.hitbox_height >= tile.pos[1] + tile.height && player.hitbox_pos[1] <= tile.pos[1] + tile.height)
		return true;
	else
		return false;
}

/*
 * @brief 	Verifie si le joueur est en collision avec le point de fin pour finir le niveau
 */
bool checkEndingPointCollision(void)
{
	tile_t * tiles = getTiles();
	for(uint8_t i=0; i< *getLevelSize(); i++)
	{
		if(tiles[i].role == ENDING_POINT)
		{
			//Regarde s'il n'y a pas collision
			if((tiles[i].pos[0] >= player.hitbox_pos[0] + player.hitbox_width)    	// trop à droite
			|| (tiles[i].pos[0] + tiles[i].width <= player.hitbox_pos[0]) 			// trop à gauche
			|| (tiles[i].pos[1] > player.hitbox_pos[1] + player.hitbox_height)		// trop en bas
			|| (tiles[i].pos[1] + tiles[i].height < player.hitbox_pos[1]))  		// trop en haut
			{}
			else
			{
				return true;
			}
		}
	}
	return false;
}

/*
 * @brief 	Update the player status for the animation
 */
void updatePlayerStatus(void)
{
	if(physStatus.onGround)
	{
		if(playerStatus == LAND)
		{
			// Coupe l'animation LAND quand le joueur se déplace en x pour que ce soit plus fluide
			if(getIndexAnim() > 2 && player.speed_x != 0)
				playerStatus = RUN;
			else
				playerStatus = LAND;
		}
		else if(playerStatus == FALL)
			playerStatus = LAND;
		else
		{
			if(player.speed_x == 0)
				playerStatus = IDLE;
			else
				playerStatus = RUN;
		}
	}
	else
	{
		if(player.speed_y < 0)
			playerStatus = JUMP;
		else
			playerStatus = FALL;
	}
	if(physStatus.onCeiling && player.speed_y > 0)
		physStatus.onCeiling = false;
	else if(physStatus.onGround && player.speed_y < 0)
		physStatus.onGround = false;
	if(physStatus.onRight && player.speed_x > 0)
		physStatus.onRight = false;
	else if(physStatus.onLeft && player.speed_x < 0)
		physStatus.onLeft = false;
}

/*
 * @brief 	Draw the player
 */
void drawPlayer(void)
{
	int16_t posX = player.hitbox_pos[0];
	int16_t posY = player.hitbox_pos[1];

	if(posX < 0 || posY < 0)
	{
		posX = (posX<0)?0:posX;
		posY = (posY<0)?0:posY;
	}

	// Efface l'image precedente du joueur
	ILI9341_DrawFilledRectangle(player.prev_pos_x, player.prev_pos_y, player.prev_pos_x + player.width, player.prev_pos_y + player.height-1, ILI9341_COLOR_WHITE);
	// Affiche la nouvelle
	ILI9341_putImage(posX, posY,25,30, stateMachine_animation(playerStatus, physStatus.facingRight),750);

	player.prev_pos_x = posX;
	player.prev_pos_y = posY;
}

/*
 * @brief 	Update the player
 */
void updatePlayer(void)
{
	update_playerMovement();
	checkCollision();
	updatePlayerStatus();
	if(checkEndingPointCollision())
		if(getIndexLevel() < getNbLevels()-1)
		{
			nextLevel();
			set_state(LOADING_NEXT_LEVEL);
		}
		else
			set_state(LOADING_WIN);
	checkDeath();
}
