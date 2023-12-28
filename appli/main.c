/**
  ******************************************************************************
  * @file    main.c
  * @author  Arnaud Morillon
  * @date    October-2023
  * @brief   Default main function.
  ******************************************************************************
*/

#include <map.h>
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
#include "tile.h"
#include "digital_button.h"
#include "pause.h"

// #define GPIO_PIN_BP_BREAK	GPIO_PIN_10

typedef enum
{
	INIT = 0,
	MENU,
	PAUSE_MENU,
	PLAY,
	LOADING_MENU,
	LOADING_GAME
}state_e;

static state_e state = MENU;

void display_update(void);
void set_state(state_e new_state);

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
 * @brief Met à jour la position du joueur
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
 * @note 	la variable t_loadPage permet d'attendre entre les différents états du jeu  
 */
void process_updateCD_ms(void)
{
	static volatile uint16_t t_jumpCD = 0;
	if(getCooldown()->hasJumped){
		if(t_jumpCD < getCooldown()->jumpCD)
			t_jumpCD++;
		else {
			// getCooldown()->hasJumped = false;
			getCooldown()->doubleJumpAvailable = true;
			getCooldown()->hasJumped = false;
			t_jumpCD = 0;
		}
	}
	/*
	static volatile uint16_t t_shootCD = 0;
	if(getCooldown()->hasShot){
		if(t_shootCD < getCooldown()->shootCD)
			t_shootCD++;
		else {
			getCooldown()->hasShot = false;
			t_shootCD = 0;
		}
	}
	*/
}

/*
 * @brief Met à jour la position du joueur
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
		// Variable pour l'entrée dans les états
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
					ILI9341_PutBigs(20, 30, "Platformer", &Font_7x10, 0x2222, ILI9341_COLOR_WHITE, 4, 4);
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
					//Affiche le titre "Platformer"
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
				}
				break;

			case LOADING_MENU:
				remove_callbacks();
				entrance = true;
				state = PAUSE_MENU;
				break;

			case LOADING_GAME:
				ILI9341_Fill(ILI9341_COLOR_WHITE);
				drawGround();
				Systick_add_callback_function(&process_display_ms);
				Systick_add_callback_function(&process_updatePlayer_ms);
				Systick_add_callback_function(&process_updateCD_ms);
				entrance = true;
				state = PLAY;
				break;

			default:
				break;
		}
	}
}

/*
 * @brief Supprime les fonctions de callback de systick
 */
void remove_callbacks(void)
{
	Systick_remove_callback_function(&process_checkTouchForPause_ms);
	Systick_remove_callback_function(&process_display_ms);
	Systick_remove_callback_function(&process_updatePlayer_ms);
	Systick_remove_callback_function(&process_updateCD_ms);
}

/*
 * @brief Affiche les elements du jeu
 */
void display_update(void)
{
	// player
	drawPlayer();
	// bullets
	// drawBullets();
	// coins
}

/*
 * @brief Change l'etat du jeu
 */
void set_state(state_e new_state)
{
	state = new_state;
}
