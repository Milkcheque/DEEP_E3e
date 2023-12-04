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

void initAnim();
uint16_t * transformTransparency(uint16_t * image);
uint16_t * getAnim(playerStatus_e animation);

uint16_t * getRun();

#endif /* ANIMATION_H_ */
