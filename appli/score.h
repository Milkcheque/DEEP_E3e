/*
 * score.h
 *
 *  Created on: 12 jan. 2024
 *      Author: Arnaud Morillon
 */
#include "macro_types.h"

#ifndef SCORE_H_
    #define SCORE_H_

void incrementChrono(void);
void resetChrono(void);
void drawChrono_inGame(void);
void drawChrono_scoreboard(bool_e hasWon);

#endif /* SCORE_H_ */