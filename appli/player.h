/*
 * @file player.h
 * @author Arnaud Morillon
 * @date 22 nov. 2023
 * @brief Contient toutes les variables qui concernent directement le joueur
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include <stdbool.h>
#include "macro_types.h"

/*
 * @brief 	Etats du joueur pour les animations et les machines à etats
 */
typedef enum{
	IDLE,
	RUN,
	JUMP,
	FALL,
	LAND,
	SHOOT,
}playerStatus_e;

typedef struct {
	bool onGround;
	bool onCeiling;
	bool onLeft;
	bool onRight;
	bool facingRight;
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
	int8_t jumpSpeed;
	int8_t dashSpeed;
	int8_t health;
	int16_t hitbox_pos[2];
	int8_t hitbox_width;
	int8_t hitbox_height;
} player_t;

typedef struct {
	bool jumpAvailable;
	bool hasJumped;
	bool doubleJumpAvailable;
	bool hasDoubleJumped;
	uint16_t jumpCD;
	bool hasDashed;
	bool isDashing;
	uint16_t dashCD;
} cooldown_t;

void initPlayer(void);
void setPosPlayer(uint16_t x, uint16_t y);
void setPlayerStatus(playerStatus_e state);
player_t * getPlayer(void);
bool getFacingRight(void);
cooldown_t * getCooldown(void);
void update_playerMovement(void);
void drawPlayer(void);
void updatePlayer(void);
bool checkEndingPointCollision(void);

#endif /* PLAYER_H_ */
