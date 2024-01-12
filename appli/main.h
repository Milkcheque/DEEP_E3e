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
	WIN,
	LOADING_MENU,
	LOADING_GAME,
	LOADING_DEATH,
	LOADING_WIN,
	SCORE_MENU
}state_e;

void set_state(state_e new_state);