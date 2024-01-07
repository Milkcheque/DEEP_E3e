/*
 * main.h
 *
 *  Created on: 7 jan. 2024
 *      Author: Arnaud Morillon
 */

 typedef enum
{
	INIT = 0,
	MENU,
	PAUSE_MENU,
	PLAY,
	DEATH,
	LOADING_MENU,
	LOADING_GAME,
	LOADING_DEATH,
	SCORE_MENU
}state_e;

void set_state(state_e new_state);