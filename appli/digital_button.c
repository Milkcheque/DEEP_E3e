/*
 * digital_button.c
 *
 *  Created on: 20 dec. 2023
 *      Author: Arnaud Morillon
 */

#include "digital_button.h"
#include "stm32f1_fonts.h"
#include "stm32f1_ili9341.h"

static button_t pauseMenu_buttons[2];
static button_t play_button;

/*
 * @brief Initialise les boutons
 */
void button_init(void)
{
    //Initialisation menu principal
    sprintf(play_button.text,"JOUER");
    //Calcul de la taille du texte
    ILI9341_GetStringSize(play_button.text, &Font_7x10, &play_button.fontWidth, &play_button.fontHeight);
    play_button.bigger = 4;
    play_button.fontWidth *= play_button.bigger; //On multiplie par 4 pour avoir une taille de police plus grande pour ILI9341_PutBigs()
    play_button.fontHeight *= play_button.bigger;
    play_button.width = play_button.fontWidth + 40;
    play_button.height = play_button.fontHeight + 30;
    play_button.x = 320/2 - play_button.width/2;
    play_button.y = 240*2/3 - play_button.height/2;


    //Initialisation menu pour pause
    //initialisation bouton resume
    button_t resume_button = {
        .x = 15,
        .y = 140,
        .width = 140,
        .height = 90,
        .bigger = 2,
        .text = "REPRENDRE"
    };

    //initialisation bouton EXIT
    button_t exit_button = {
        .x = 170,
        .y = 140,
        .width = 140,
        .height = 90,
        .bigger = 2,
        .text = "QUITTER"
    };
    pauseMenu_buttons[0] = resume_button;
    pauseMenu_buttons[1] = exit_button;
}

/*
 * @brief Retourne le bouton souhaité du menu de pause
 * @param index : indice du bouton dans le tableau pauseMenu_buttons
 */
button_t get_pauseMenuButton(uint8_t index)
{
    return pauseMenu_buttons[index];
}

/*
 * @brief Dessine le bouton du menu principal
 */
void draw_menuButton(void)
{
    ILI9341_DrawFilledRectangle(play_button.x, play_button.y, play_button.x+play_button.width, play_button.y+play_button.height, 0x2222);
    ILI9341_PutBigs(play_button.x + 20, play_button.y + 30, play_button.text, &Font_7x10, ILI9341_COLOR_WHITE, 0x2222, play_button.bigger, play_button.bigger);
}

/*
 * @brief Dessine les boutons du menu pause
 */
void draw_pauseMenuButtons(void)
{
    for(uint8_t i = 0; i < 2; i++)
    {
        ILI9341_DrawFilledRectangle(pauseMenu_buttons[i].x, pauseMenu_buttons[i].y, pauseMenu_buttons[i].x + pauseMenu_buttons[i].width, pauseMenu_buttons[i].y + pauseMenu_buttons[i].height, 0x2222);
        ILI9341_PutBigs(pauseMenu_buttons[i].x + 5, pauseMenu_buttons[i].y + 30, pauseMenu_buttons[i].text, &Font_7x10, ILI9341_COLOR_WHITE, 0x2222, pauseMenu_buttons[i].bigger, pauseMenu_buttons[i].bigger);
    }
    // ILI9341_PutBigs(15, 150, "PLAY", &Font_11x18, ILI9341_COLOR_WHITE, 0x2222, 3, 3);
    // ILI9341_PutBigs(180, 150, "EXIT", &Font_11x18, ILI9341_COLOR_WHITE, 0x2222, 3, 3);
}
