/**
  ******************************************************************************
  * @file    main.c
  * @author  Nirgal
  * @date    03-July-2019
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

void display_update(void);

/*
 * @brief Met à jour l'affichage
 */
void process_display_ms(void)
{
	static volatile uint32_t t_FPS = 30;
	if(t_FPS)
		t_FPS--;
	if(t_FPS <= 0){
		t_FPS = 30;
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
		t_input = 20;
		updatePlayer();
	}
}

/*
 * @brief Met à jour les cooldowns du joueur
 */
void process_updateCD_ms(void)
{
	static volatile uint16_t t_jumpCD = 0;
	static volatile uint16_t t_shootCD = 0;
	if(getCooldown()->hasJumped){
		if(t_jumpCD < getCooldown()->jumpCD)
			t_jumpCD++;
		else {
			getCooldown()->hasJumped = false;
			t_jumpCD = 0;
		}
	}
	/*
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

	//On ajoute la fonction process_ms � la liste des fonctions appel�es automatiquement chaque ms par la routine d'interruption du p�riph�rique SYSTICK
	//Systick_add_callback_function(&process_ms);

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
		typedef enum
		{
			INIT = 0,
			MENU,
			PLAY,
			BREAK
		}state_e;

		static int16_t static_x,static_y;
		int16_t x, y;

		static state_e state = INIT;
		switch(state)
		{
			case INIT:
				initMap();		//Initialisation de la map
				initPlayer();	//Initialisation du joueur
				initAnim();		//Inialisation des animations
				Systick_add_callback_function(&process_display_ms);
				Systick_add_callback_function(&process_updatePlayer_ms);
				Systick_add_callback_function(&process_updateCD_ms);
				state = PLAY;
				break;

			case MENU:
				/*if(XPT2046_getMedianCoordinates(&x, &y, XPT2046_COORDINATE_SCREEN_RELATIVE))
				{
					Systick_add_callback_function(&process_display_ms);
					Systick_add_callback_function(&process_updatePlayer_ms);
					Systick_add_callback_function(&process_updateCD_ms);

					ILI9341_DrawCircle(static_x,static_y,15,ILI9341_COLOR_WHITE);
					ILI9341_DrawCircle(x,y,15,ILI9341_COLOR_BLUE);
					static_x = x;
					static_y = y;

					state = PLAY;
				}
				*/
				break;

			case PLAY:
				/*if(XPT2046_getMedianCoordinates(&x, &y, XPT2046_COORDINATE_SCREEN_RELATIVE))
				{
					Systick_remove_callback_function(process_display_ms);
					Systick_remove_callback_function(process_updatePlayer_ms);
					Systick_remove_callback_function(process_updateCD_ms);

					ILI9341_DrawCircle(static_x,static_y,15,ILI9341_COLOR_WHITE);
					ILI9341_DrawCircle(x,y,15,ILI9341_COLOR_BLUE);
					static_x = x;
					static_y = y;

					state = MENU;
				}
				*/
				//updatePlayer();

				break;

			default:
				break;
		}
	}
}

/*
 * @brief Affiche les éléments du jeu
 */
void display_update(void)
{
	// background
	// obstacles
	drawGround();////////////////////
	// player
	drawPlayer();
	// enemies
	// bullets 
}
