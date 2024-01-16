/**
  ******************************************************************************
  * @file    main.c
  * @author  Arnaud Morillon
  * @date    October-2023
  * @brief   Default main function.
  ******************************************************************************
*/

#include "main.h"
#include "map.h"
#include "stm32f1xx_hal.h"
#include "stm32f1_uart.h"
#include "stm32f1_sys.h"
#include "stm32f1_gpio.h"
#include "macro_types.h"
#include "systick.h"
#include "tft_ili9341/stm32f1_ili9341.h"
#include "stm32f1_ili9341.h"
#include "stm32f1_xpt2046.h"
#include "stm32f1_adc.h"
#include "player.h"
#include "animation.h"
#include "digital_button.h"
#include "score.h"

static state_e state = MENU;
void display_update(void);
bool_e checkScreenTouch(void);


/*
 * @brief Calcule le temps d'une partie
 */
void process_chrono_ms(void)
{
	static volatile uint32_t t_chrono = 1000;
	if(t_chrono)
		t_chrono--;
	if(t_chrono <= 0){
		t_chrono = 1000;
		incrementChrono();
		drawChrono_inGame();
	}
}

/*
 * @brief Met à jour l'affichage
 */
void process_display_ms(void)
{
	static volatile uint32_t t_FPS = 30;
	if(t_FPS)
		t_FPS--;
	if(t_FPS <= 0){
		t_FPS = 50;
		display_update();
	}
}

/*
 * @brief Met à jour le joueur
 */
void process_updatePlayer_ms(void)
{
	static volatile uint32_t t_input = 0;
	if(t_input)
		t_input--;
	if(t_input <= 0){
		t_input = 40;
		updatePlayer();
	}
}

/*
 * @brief Met à jour les cooldowns du joueur
 */
void process_updateCD_ms(void)
{
	static volatile uint16_t t_jumpCD = 0;
	if(getCooldown()->hasJumped){
		if(t_jumpCD < getCooldown()->jumpCD)
			t_jumpCD++;
		else {
			getCooldown()->doubleJumpAvailable = true;
			getCooldown()->hasJumped = false;
			t_jumpCD = 0;
		}
	}
	static volatile uint16_t t_dashCD = 0;
	if(getCooldown()->hasDashed){
		if(t_dashCD < getCooldown()->dashCD)
			t_dashCD++;
		else {
			getCooldown()->hasDashed = false;
			t_dashCD = 0;
		}
	}
	else if(getCooldown()->isDashing){
		if(t_dashCD < getCooldown()->dashCD)
			t_dashCD += 16;
		else {
			getCooldown()->hasDashed = true;
			getCooldown()->isDashing = false;
			t_dashCD = 0;
		}
	}
}

/*
 * @brief Verifie si l'ecran est touché pour mettre le jeu en pause
 */
void process_checkTouchForPause_ms(void)
{
	static volatile uint32_t t_touch = 0;
	if(t_touch)
		t_touch--;
	if(t_touch <= 0){
		t_touch = 15;
		if(checkScreenTouch())
		{
			set_state(LOADING_MENU);
		}
	}
}

/*
 * @brief Fonction principale
 */
int main(void)
{
	//Initialisation de la couche logicielle HAL (Hardware Abstraction Layer)
	//Cette ligne doit rester la premi�re �tape de la fonction main().

	HAL_Init();

	//Initialisation de l'UART2 � la vitesse de 115200 bauds/secondes (92kbits/s) PA2 : Tx  | PA3 : Rx.
		//Attention, les pins PA2 et PA3 ne sont pas reli�es jusqu'au connecteur de la Nucleo.
		//Ces broches sont redirig�es vers la sonde de d�bogage, la liaison UART �tant ensuite encapsul�e sur l'USB vers le PC de d�veloppement.
	//UART_init(UART2_ID,115200);

	//"Indique que les printf sortent vers le p�riph�rique UART2."
	//SYS_set_std_usart(UART2_ID, UART2_ID, UART2_ID);

	UART_init(UART2_ID,115200);
	SYS_set_std_usart(UART2_ID, UART2_ID, UART2_ID);

	//Initialisation de l'ADC
	ADC_init();

	//Initialisation de l'ï¿½cran tft
	ILI9341_Init();
	ILI9341_Rotate(ILI9341_Orientation_Landscape_2);
	ILI9341_Fill(ILI9341_COLOR_WHITE);

	//initialisation du tactile
	XPT2046_init();

	while(1){
		// Variable pour l'entree dans les etats
		static bool entrance = true;

		// Variables pour la position du toucher tactile
		int16_t x, y;

		switch(state)
		{
			case INIT:
				initMap();		//Initialisation de la map
				initPlayer();	//Initialisation du joueur
				initAnim();		//Inialisation des animations
				state = LOADING_GAME;
				entrance = true;
				break;

			case MENU:
				if(entrance)
				{
					entrance = false;
					ILI9341_Fill(ILI9341_COLOR_WHITE);
					button_init();
					draw_menuButton();

					//Affiche le titre "Platformer"
					ILI9341_PutBigs(20, 40, "Platformer", &Font_7x10, 0x2222, ILI9341_COLOR_WHITE, 4, 4);
					
					resetChrono();
					resetLevel();
				}

				if(XPT2046_getMedianCoordinates(&x, &y, XPT2046_COORDINATE_SCREEN_RELATIVE))
				{
					state = INIT;
					entrance = true;
				}
				break;
			
			case PAUSE_MENU:
				if(entrance)
				{
					entrance = false;
					ILI9341_Fill(ILI9341_COLOR_WHITE);
					button_init();
					draw_pauseMenuButtons();
					ILI9341_PutBigs(35, 40, "jeu en pause", &Font_7x10, 0x2222, ILI9341_COLOR_WHITE, 3, 3);
				}

				if(XPT2046_getMedianCoordinates(&x, &y, XPT2046_COORDINATE_SCREEN_RELATIVE))
				{
					if(x < get_pauseMenuButton(0).x + get_pauseMenuButton(0).width && x > get_pauseMenuButton(0).x && y < get_pauseMenuButton(0).y + get_pauseMenuButton(0).height && y > get_pauseMenuButton(0).y)
					{
						state = LOADING_GAME;
					}

					else if(x < get_pauseMenuButton(1).x + get_pauseMenuButton(1).width && x > get_pauseMenuButton(1).x && y < get_pauseMenuButton(1).y + get_pauseMenuButton(1).height && y > get_pauseMenuButton(1).y)
					{
						state = MENU;
						remove_callbacks();
						entrance = true;
					}
				}
				break;

			case PLAY:
				if(entrance)
				{
					entrance = false;
					Systick_add_callback_function(&process_checkTouchForPause_ms);
					Systick_add_callback_function(&process_chrono_ms);
				}
				break;

			case LOADING_MENU:
				remove_callbacks();
				entrance = true;
				state = PAUSE_MENU;
				break;

			case LOADING_GAME:
				ILI9341_Fill(ILI9341_COLOR_WHITE);
				drawMap();
				Systick_add_callback_function(&process_display_ms);
				Systick_add_callback_function(&process_updatePlayer_ms);
				Systick_add_callback_function(&process_updateCD_ms);
				entrance = true;
				state = PLAY;
				break;
			
			case LOADING_NEXT_LEVEL:
				remove_callbacks();
				state = INIT;
				break;

			case LOADING_DEATH:
				remove_callbacks();
				entrance = true;
				state = DEATH;
				break;
			
			case LOADING_WIN:
				remove_callbacks();
				entrance = true;
				state = WIN;
				break;

			case DEATH:
				if(entrance)
				{
					entrance = false;
					ILI9341_Fill(ILI9341_COLOR_WHITE);
					ILI9341_PutBigs(35, 20, "Chute fatale", &Font_7x10, ILI9341_COLOR_RED, ILI9341_COLOR_WHITE, 3, 3);
					ILI9341_PutBigs(105, 90, "Score", &Font_7x10, 0x2222, ILI9341_COLOR_WHITE, 3, 3);
					drawChrono_scoreboard(0);
					ILI9341_PutBigs(30, 200, "< Menu principal >", &Font_7x10, 0x2222, ILI9341_COLOR_WHITE, 2, 2);
				}

				if(XPT2046_getMedianCoordinates(&x, &y, XPT2046_COORDINATE_SCREEN_RELATIVE))
				{
					state = MENU;
					entrance = true;
				}
				break;
			
			case WIN:
				if(entrance)
				{
					entrance = false;
					ILI9341_Fill(ILI9341_COLOR_WHITE);
					ILI9341_PutBigs(45, 30, "Victoire !!", &Font_7x10, ILI9341_COLOR_BLUE, ILI9341_COLOR_WHITE, 3, 3);
					ILI9341_PutBigs(105, 90, "Score", &Font_7x10, 0x2222, ILI9341_COLOR_WHITE, 3, 3);
					drawChrono_scoreboard(1);
					ILI9341_PutBigs(30, 200, "< Menu principal >", &Font_7x10, 0x2222, ILI9341_COLOR_WHITE, 2, 2);
				}

				if(XPT2046_getMedianCoordinates(&x, &y, XPT2046_COORDINATE_SCREEN_RELATIVE))
				{
					state = MENU;
					entrance = true;
				}
				break;

			default:
				break;
		}
	}
}

/*
 * @brief Supprime les fonctions du callback de systick
 */
void remove_callbacks(void)
{
	Systick_remove_callback_function(&process_checkTouchForPause_ms);
	Systick_remove_callback_function(&process_display_ms);
	Systick_remove_callback_function(&process_updatePlayer_ms);
	Systick_remove_callback_function(&process_updateCD_ms);
	Systick_remove_callback_function(&process_chrono_ms);
}

/*
 * @brief Affiche les elements du jeu
 */
void display_update(void)
{
	drawPlayer();
}

bool_e checkScreenTouch(void)
{
    uint8_t x, y;
    if(XPT2046_getCoordinates(&x, &y, XPT2046_COORDINATE_SCREEN_RELATIVE))
        return 1;
    else
        return 0;
}

/*
 * @brief Change l'etat du jeu
 */
void set_state(state_e new_state)
{
	state = new_state;
}
