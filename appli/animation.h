/*
 * image.h
 *
 *  Created on: 17 nov. 2023
 *      Author: Arnaud Morillon
 */
#include "macro_types.h"
#include "player.h"

#ifndef ANIMATION_H_
#define ANIMATION_H_

void initAnim(void);
void incrementIndexAnim(void);
uint16_t flipImageY(uint16_t *image, int width, int height);
uint16_t transformTransparency(uint16_t * image);
uint16_t * getAnim(playerStatus_e animation);
uint16_t *  stateMachine_animation(playerStatus_e state);


#endif /* ANIMATION_H_ */
