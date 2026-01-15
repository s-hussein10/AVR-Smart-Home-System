/*
 * DIO_program.c
 *
 *  Created on: Aug 23, 2025
 *      Author: mohamed.khaled
 */


#include "../../LIBS/STD_TYPES.h"
#include "../../LIBS/BIT_MATH.h"

#include "DIO_interface.h"
#include "DIO_private.h"
#include "DIO_config.h"


void DIO_voidSetPinDirection	(u8 Copy_u8Port, u8 Copy_u8Pin, u8 Copy_u8Direction)
{
	switch(Copy_u8Port)
	{
	case DIO_PORTA :
		switch(Copy_u8Direction)
		{
		case DIO_INPUT :
				CLR_BIT(DDRA_REG, Copy_u8Pin);
			break;
		case DIO_OUTPUT :
				SET_BIT(DDRA_REG, Copy_u8Pin);
			break;
		default :
			break;
		}
		break;
	case DIO_PORTB :
		switch(Copy_u8Direction)
				{
				case DIO_INPUT :
						CLR_BIT(DDRB_REG, Copy_u8Pin);
					break;
				case DIO_OUTPUT :
						SET_BIT(DDRB_REG, Copy_u8Pin);
					break;
				default :
					break;
				}
			break;
	case DIO_PORTC :
		switch(Copy_u8Direction)
				{
				case DIO_INPUT :
						CLR_BIT(DDRC_REG, Copy_u8Pin);
					break;
				case DIO_OUTPUT :
						SET_BIT(DDRC_REG, Copy_u8Pin);
					break;
				default :
					break;
				}
			break;
	case DIO_PORTD :
		switch(Copy_u8Direction)
				{
				case DIO_INPUT :
						CLR_BIT(DDRD_REG, Copy_u8Pin);
					break;
				case DIO_OUTPUT :
						SET_BIT(DDRD_REG, Copy_u8Pin);
					break;
				default :
					break;
				}
			break;
	default:
		break;
	}
}
void DIO_voidSetPinValue		(u8 Copy_u8Port, u8 Copy_u8Pin, u8 Copy_u8Value)
{
	switch(Copy_u8Port)
		{
		case DIO_PORTA :
			switch(Copy_u8Value)
			{
			case DIO_LOW :
					CLR_BIT(PORTA_REG, Copy_u8Pin);
				break;
			case DIO_HIGH :
					SET_BIT(PORTA_REG, Copy_u8Pin);
				break;
			default :
				break;
			}
			break;
		case DIO_PORTB :
			switch(Copy_u8Value)
					{
					case DIO_LOW :
							CLR_BIT(PORTB_REG, Copy_u8Pin);
						break;
					case DIO_HIGH :
							SET_BIT(PORTB_REG, Copy_u8Pin);
						break;
					default :
						break;
					}
				break;
		case DIO_PORTC :
			switch(Copy_u8Value)
					{
					case DIO_LOW :
							CLR_BIT(PORTC_REG, Copy_u8Pin);
						break;
					case DIO_HIGH :
							SET_BIT(PORTC_REG, Copy_u8Pin);
						break;
					default :
						break;
					}
				break;
		case DIO_PORTD :
			switch(Copy_u8Value)
					{
					case DIO_LOW :
							CLR_BIT(PORTD_REG, Copy_u8Pin);
						break;
					case DIO_HIGH :
							SET_BIT(PORTD_REG, Copy_u8Pin);
						break;
					default :
						break;
					}
				break;
		default:
			break;
		}
}



u8	 DIO_u8GetPinValue			(u8 Copy_u8Port, u8 Copy_u8Pin)
{
	u8 Local_u8ReturnValue;
	switch(Copy_u8Port)
	{
	case DIO_PORTA :
			Local_u8ReturnValue = GET_BIT(PINA_REG, Copy_u8Pin);
		break;
	case DIO_PORTB :
			Local_u8ReturnValue = GET_BIT(PINB_REG, Copy_u8Pin);
		break;
	case DIO_PORTC :
			Local_u8ReturnValue = GET_BIT(PINC_REG, Copy_u8Pin);
		break;
	case DIO_PORTD :
			Local_u8ReturnValue = GET_BIT(PIND_REG, Copy_u8Pin);
		break;
	default :
		break;
	}


	return Local_u8ReturnValue;
}



void DIO_voidSetPortDirection	(u8 Copy_u8Port, u8 Copy_u8Direction)
{
	switch (Copy_u8Port)
		{
		case DIO_PORTA:
			switch(Copy_u8Direction)
			{
			case DIO_OUTPUT :
					DDRA_REG = 0xff;
				break;
			case DIO_INPUT :
					DDRA_REG = 0x00;
				break;
			default :
				break;
			}
			break;
		case DIO_PORTB:
			switch(Copy_u8Direction)
						{
						case DIO_OUTPUT :
								DDRB_REG = 0xff;
							break;
						case DIO_INPUT :
								DDRB_REG = 0x00;
							break;
						default :
							break;
						}
				break;
		case DIO_PORTC:
			switch(Copy_u8Direction)
						{
						case DIO_OUTPUT :
								DDRC_REG = 0xff;
							break;
						case DIO_INPUT :
								DDRC_REG = 0x00;
							break;
						default :
							break;
						}
				break;
		case DIO_PORTD:
			switch(Copy_u8Direction)
						{
						case DIO_OUTPUT :
								DDRD_REG = 0xff;
							break;
						case DIO_INPUT :
								DDRD_REG = 0x00;
							break;
						default :
							break;
						}
				break;
		}
}
void DIO_voidSetPortValue		(u8 Copy_u8Port, u8 Copy_u8Value)
{
	switch (Copy_u8Port)
		{
		case DIO_PORTA:
			PORTA_REG = Copy_u8Value;
			break;
		case DIO_PORTB:
			PORTB_REG = Copy_u8Value;
				break;
		case DIO_PORTC:
			PORTC_REG = Copy_u8Value;
				break;
		case DIO_PORTD:
			PORTD_REG = Copy_u8Value;
				break;
		}
}

u8	 DIO_u8GetPortValue			(u8 Copy_u8Port)
{
	u8 Local_u8ReturnedValue;
	switch (Copy_u8Port)
	{
	case DIO_PORTA:
		Local_u8ReturnedValue = PINA_REG;
		break;
	case DIO_PORTB:
		Local_u8ReturnedValue = PINB_REG;
			break;
	case DIO_PORTC:
		Local_u8ReturnedValue = PINC_REG;
			break;
	case DIO_PORTD:
		Local_u8ReturnedValue = PIND_REG;
			break;
	}

	return Local_u8ReturnedValue;
}




