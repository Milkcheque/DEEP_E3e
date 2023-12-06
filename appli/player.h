/*
 * player.h
 *
 *  Created on: 22 nov. 2023
 *      Author: Arnaud Morillon
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include <stdbool.h>
#include "macro_types.h"

//extern

/*
 * @brief 	Etats du joueur pour les animations et les machines à etats
 */
typedef enum{
	IDLE,
	RUN,
	JUMP,
	FALL,
	SHOOT,
	DAMAGED,
	DEATH
}playerStatus_e;

typedef struct {
	bool onGround;
	bool onCeiling;
	bool onLeft;
	bool onRight;
}physicalStatus_t;

typedef struct {
	playerStatus_e status;
	int16_t pos_x;
	int16_t pos_y;
	int16_t prev_pos_x;
	int16_t prev_pos_y;
	int8_t width;
	int8_t height;
	int8_t speed_x;
	int8_t speed_y;
	int8_t health;
	int16_t hitbox_pos[2];
	int8_t hitbox_width;
	int8_t hitbox_height;
} player_t;

typedef struct {
	bool hasJumped;
	uint16_t jumpCD;
	bool hasShot;
	uint16_t shootCD;
} cooldown_t;

void initPlayer(void);
void setPosPlayer(uint16_t x, uint16_t y);
player_t * getPlayer(void);
cooldown_t * getCooldown(void);
void update_playerMovement(void);
void drawPlayer(void);
void updatePlayer(void);

#endif /* PLAYER_H_ */
