/*
 * score.c
 *
 *  Created on: 12 jan. 2024
 *      Author: Arnaud Morillon
 */

#include "score.h"
#include "stm32f1_fonts.h"
#include "stm32f1_ili9341.h"

// Chrono en secondes
static uint16_t chrono = 0;

/*
 * @brief Incremente le temps d'une partie
 */
void incrementChrono(void)
{
    chrono++;
}

/*
 * @brief Reset le chrono
 */
 void resetChrono(void)
 {
     chrono = 0;
 }

 /*
 * @brief 	Draw the player
 */
void drawChrono_inGame(void)
{
	uint8_t minutes = (uint8_t)(chrono / 60);
    uint8_t secondes = (uint8_t)(chrono % 60);
    if(secondes < 10)
        ILI9341_printf(0, 0, &Font_11x18, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK, "%d:0%d", minutes, secondes);
    else
        ILI9341_printf(0, 0, &Font_11x18, ILI9341_COLOR_WHITE, ILI9341_COLOR_BLACK, "%d:%d", minutes, secondes);
}
 
 /*
 * @brief 	Draw the player
 */
void drawChrono_scoreboard(bool_e hasWon)
{
    if(hasWon)
    {
        uint8_t minutes = (uint8_t)(chrono / 60);
        uint8_t secondes = (uint8_t)(chrono % 60);
        if(secondes < 10)
            ILI9341_printf(130, 130, &Font_16x26, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE, "%d:0%d", minutes, secondes);
        else
            ILI9341_printf(130, 130, &Font_16x26, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE, "%d:%d", minutes, secondes);
            // ILI9341_PutBigs(35, 100, "Chute fatale", &Font_7x10, ILI9341_COLOR_RED, ILI9341_COLOR_WHITE, 3, 3);
    }
    else
        ILI9341_printf(117, 130, &Font_16x26, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE, "--:--");
}

