/*
 * joystick.c
 *
 *  Created on: 15 nov. 2023
 *      Author: Arnaud Morillon
 */
#include "joystick.h"
#include "stm32f1_adc.h"

#define ADC_JOYSTICK_X_CHANNEL	ADC_0
#define ADC_JOYSTICK_Y_CHANNEL	ADC_1

//UTILISER extern ???


uint16_t getX_Axis_move(){
	uint16_t X = ADC_getValue(ADC_JOYSTICK_X_CHANNEL);
	if(X < 2000){
		return -1*(4095-X/4095)*10; //utiliser speed du player

	} else if(X > 2120){
		return (X/4095)*10;
	}
}
