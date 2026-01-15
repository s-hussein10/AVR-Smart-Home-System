
#include "../../LIBS/STD_TYPES.h"
#include "../../LIBS/BIT_MATH.h"

#define F_CPU 8000000UL
#include "avr/delay.h"

/*MCAL*/
#include "../../MCAL/DIO/DIO_interface.h"

/*CLCD*/
#include "CLCD_interface.h"
#include "CLCD_private.h"
#include "CLCD_config.h"


void CLCD_voidInit	(void)
{
	// Data port --> output
	DIO_voidSetPortDirection(CLCD_DATA_PORT, DIO_OUTPUT);
	// RS , RW , EN --> output
	DIO_voidSetPinDirection(CLCD_CTRL_PORT, CLCD_RS_PIN, DIO_OUTPUT);
	DIO_voidSetPinDirection(CLCD_CTRL_PORT, CLCD_RW_PIN, DIO_OUTPUT);
	DIO_voidSetPinDirection(CLCD_CTRL_PORT, CLCD_EN_PIN, DIO_OUTPUT);

	/*Initialization sequence */
	_delay_ms(50);
	/*Function set*/
	CLCD_voidSendCommand(0b00111000);
	_delay_us(50);
	/*Display ON/OFF */
	CLCD_voidSendCommand(0b00001100);
	_delay_us(50);
	/*Display Clear */
	CLCD_voidSendCommand(CLCD_CLEAR_COMMAND);
	_delay_ms(2);
	/*Entry Mode Set*/
	CLCD_voidSendCommand(0b00000110);
}

void CLCD_voidSendCommand 	(u8 Copy_u8Command)
{
	/*RS --> LOW ( To send command) */
	DIO_voidSetPinValue(CLCD_CTRL_PORT, CLCD_RS_PIN, DIO_LOW);
	/*RW -> Low (To Write) */
	DIO_voidSetPinValue(CLCD_CTRL_PORT, CLCD_RW_PIN, DIO_LOW);
	/* Send command */
	DIO_voidSetPortValue(CLCD_DATA_PORT, Copy_u8Command);
	/* Enable sequence */
	DIO_voidSetPinValue(CLCD_CTRL_PORT, CLCD_EN_PIN, DIO_HIGH);
	_delay_us(1);
	DIO_voidSetPinValue(CLCD_CTRL_PORT, CLCD_EN_PIN, DIO_LOW);
	_delay_ms(1);
}

void CLCD_voidSendData		(u8 Copy_u8Data)
{
	/*RS --> HIGH ( To send data) */
	DIO_voidSetPinValue(CLCD_CTRL_PORT, CLCD_RS_PIN, DIO_HIGH);
	/*RW -> Low (To Write) */
	DIO_voidSetPinValue(CLCD_CTRL_PORT, CLCD_RW_PIN, DIO_LOW);
	/* Send command */
	DIO_voidSetPortValue(CLCD_DATA_PORT, Copy_u8Data);
	/* Enable sequence */
	DIO_voidSetPinValue(CLCD_CTRL_PORT, CLCD_EN_PIN, DIO_HIGH);
	_delay_us(1);
	DIO_voidSetPinValue(CLCD_CTRL_PORT, CLCD_EN_PIN, DIO_LOW);
	_delay_ms(1);
}

void CLCD_voidSetPosition	(u8 Copy_u8Row, u8 Copy_u8Col)
{
	switch(Copy_u8Row)
	{
	case CLCD_ROW_1:
			CLCD_voidSendCommand(0x80 + Copy_u8Col -1 );
		break;
	case CLCD_ROW_2:
			CLCD_voidSendCommand(0x80 + 64 + Copy_u8Col -1 );
		break;
	default:
		break;
	}
}

void CLCD_voidSendString	(u8* Copy_pu8String )
{
	u8 Local_u8Iterator = 0 ;
	while(Copy_pu8String[Local_u8Iterator] != '\0')
	{
		CLCD_voidSendData(Copy_pu8String[Local_u8Iterator]);
		Local_u8Iterator++;
	}
}

void CLCD_voidSendExtraCharacter (u8 Copy_u8PatternLocation, u8* Copy_pu8ExtraChar, u8 Copy_u8Row, u8 Copy_u8Col)
{
	u8 Local_u8Address = 0;
	Local_u8Address = Copy_u8PatternLocation * 8;
	/*Go To CGRAM */
	CLCD_voidSendCommand(64 + Local_u8Address);
	/*Write the character in CGRAM */
	for(u8 i = 0 ; i < 8 ; i++)
	{
		CLCD_voidSendData(Copy_pu8ExtraChar[i]);
	}
	/*Back To DDRAM */
	CLCD_voidSetPosition(Copy_u8Row, Copy_u8Col);
	/* Draw character on display */
	CLCD_voidSendData(Copy_u8PatternLocation);
}


void CLCD_voidSendNumber	(u64 Copy_u64Number)
{
	if(Copy_u64Number == 0)
	{
		CLCD_voidSendData('0');
	}
	else
	{
		u64 Local_u64Reversed = 1 ;
		while( Copy_u64Number != 0 )
		{
			Local_u64Reversed = (Local_u64Reversed * 10) + (Copy_u64Number % 10);
			Copy_u64Number /= 10;
		}
		// display on lcd
		while(Local_u64Reversed != 1 )
		{
			CLCD_voidSendData((Local_u64Reversed % 10) + '0');
			Local_u64Reversed /= 10;
		}
	}

}



