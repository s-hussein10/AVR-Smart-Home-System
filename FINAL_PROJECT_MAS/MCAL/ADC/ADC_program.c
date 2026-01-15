/*
 * ADC_program.c
 *
 *  Created on: 19 Sep 2025
 *      Author: Sief
 */

#include "../../LIBS/STD_TYPES.h"
#include "../../LIBS/BIT_MATH.h"

#include "ADC_interface.h"
#include "ADC_private.h"
#include "ADC_config.h"

void ADC_voidInit(void)
{
	/*
	 * ref voltage = 5v
	 * Right adjustment
	 * ADC0
	 * Enable ADC
	 * Disable PIE	(( Polling ))
	 * prescaler / 128
	 * */

	ADMUX = 	0b01000000;
	ADCSRA = 	0b10010111;
}

u16 ADC_u16ConvertedValue (u8 Copy_u8Channel)
{
	ADMUX = ADMUX & 0b11100000;
	ADMUX |= Copy_u8Channel;

	u16 Local_u16Conv = 0 ;

	// start conversion
	SET_BIT(ADCSRA, 6);

	// wait until the flag PIF (event) raised (( Polling ))
	while(GET_BIT(ADCSRA, 4) == 0 );

	// read the output registers
	Local_u16Conv = (ADCL) | (ADCH << 8);
	// clear the flag
	SET_BIT(ADCSRA, 4);

	return Local_u16Conv;

}


