#define F_CPU 8000000UL
#include <util/delay.h>

/* ===================== LIBRARIES ===================== */
#include "../LIBS/STD_TYPES.h"
#include "../LIBS/BIT_MATH.h"

/* ===================== MCAL ========================== */
#include "../MCAL/DIO/DIO_interface.h"
#include "../MCAL/GIE/GIE_interface.h"
#include "../MCAL/ADC/ADC_interface.h"
#include "../MCAL/SPI/SPI_interface.h"
#include "../MCAL/SPI/SPI_private.h"

/* ===================== HAL =========================== */
#include "../HAL/CLCD/CLCD_interface.h"

/* ===================== PROTOTYPES ==================== */
void Execute_LightSystem(u8 Mode);
u16  Get_LDR_Reading(void);
void Update_Slave_LCD(u8 Mode);

/* ===================== GLOBAL VARIABLES ============== */
/* 'A'  Automatic mode
 * 'O'  Manual ON
 * 'C'  Manual OFF
 */
volatile u8 LightMode = 'A'; // 'A' is Automatic
u8 LastMode = 0;

/* SPI Interrupt Service Routine (ISR) */
void __vector_12(void) __attribute__((signal));
void __vector_12(void) {
	u8 ReceivedData = SPDR_REG;
	if (ReceivedData == 'O' || ReceivedData == 'C' || ReceivedData == 'A')
	{
		LightMode = ReceivedData;
	}
}

int main(void)
{
	/* ================= PIN CONFIGURATION =============== */

	/* SPI Slave Pins */
	DIO_voidSetPinDirection(DIO_PORTB, DIO_PIN5, DIO_INPUT);   // MOSI
	DIO_voidSetPinDirection(DIO_PORTB, DIO_PIN6, DIO_OUTPUT);  // MISO
	DIO_voidSetPinDirection(DIO_PORTB, DIO_PIN7, DIO_INPUT);   // SCK
	DIO_voidSetPinDirection(DIO_PORTB, DIO_PIN4, DIO_INPUT);   // SS

	/* LDR and LEDs */
	DIO_voidSetPinDirection(DIO_PORTA, DIO_PIN0, DIO_INPUT);   // LDR (ADC0)
	DIO_voidSetPortDirection(DIO_PORTC, DIO_OUTPUT);          // LEDs Port

	/* ================= INITIALIZATION ================== */
	CLCD_voidInit();
	ADC_voidInit();
	SPI_voidSlaveInit();

	/* Enable SPI Interrupt */
	SET_BIT(SPCR_REG, 7); // SPIE Bit
	GIE_voidEnable();

	CLCD_voidSendString((u8*)"Slave Ready...");
	_delay_ms(1000);
	CLCD_voidSendCommand(0x01);   // Clear LCD

	u8 ReceivedData;

	while(1)
	{
		Execute_LightSystem(LightMode);

		/* Update LCD only if mode changed to avoid flickering and lag */
		if (LightMode != LastMode)
		{
			Update_Slave_LCD(LightMode);
			LastMode = LightMode;
		}
		_delay_ms(50);
	}

	/* --------- 2. Execute Light Control Logic --------- */
	Execute_LightSystem(LightMode);

	/* --------- 3. Update LCD Display --------- */
	Update_Slave_LCD(LightMode);
}


/*               LIGHT CONTROL FUNCTION                  */
/* ===================================================== */
void Execute_LightSystem(u8 Mode)
{
	u16 LDR_Val;

	if (Mode == 'O')
	{
		/* Manual ON: Turn ON all LEDs */
		DIO_voidSetPortValue(DIO_PORTC, 0xFF);
	}
	else if (Mode == 'C')
	{
		/* Manual OFF: Turn OFF all LEDs */
		DIO_voidSetPortValue(DIO_PORTC, 0x00);
	}
	else if (Mode == 'A')
	{
		/* Automatic mode based on LDR reading */
		LDR_Val = Get_LDR_Reading();

		/* LED scaling according to light intensity (0�5000 mV) */

		if(LDR_Val < 500)       DIO_voidSetPortValue(DIO_PORTC, 0x01);
		else if(LDR_Val < 1000) DIO_voidSetPortValue(DIO_PORTC, 0x03);
		else if(LDR_Val < 1500) DIO_voidSetPortValue(DIO_PORTC, 0x07);
		else if(LDR_Val < 2000) DIO_voidSetPortValue(DIO_PORTC, 0x0F);
		else if(LDR_Val < 2500) DIO_voidSetPortValue(DIO_PORTC, 0x1F);
		else if(LDR_Val < 3400) DIO_voidSetPortValue(DIO_PORTC, 0x3F);
		else if(LDR_Val < 4000) DIO_voidSetPortValue(DIO_PORTC, 0x7F);
		else if(LDR_Val < 4200) DIO_voidSetPortValue(DIO_PORTC, 0xFF);
	}
}
/*               LDR READING FUNCTION                    */
/* ===================================================== */
u16 Get_LDR_Reading(void)
{
	u16 adc_res;

	/* Read ADC value from channel 0 */
	adc_res = ADC_u16ConvertedValue(ADC_CHANNEL_0);

	/* Convert ADC reading to millivolts (0�5000 mV) */
	return (u16)(((u32)adc_res * 5000UL) / 1024UL);
}

/*               LCD UPDATE FUNCTION                     */
/* ===================================================== */
void Update_Slave_LCD(u8 Mode)
{
	CLCD_voidSetPosition(CLCD_ROW_1, 1);
	CLCD_voidSendString((u8*)"Light System:");

	CLCD_voidSetPosition(CLCD_ROW_2, 1);
	CLCD_voidSendString((u8*)"Mode:  ");

	if (Mode == 'O' )
	{
		CLCD_voidSetPosition(CLCD_ROW_2, 7);
		CLCD_voidSendString((u8*)"LEDS ON ");

	}
	else if (Mode == 'C')
	{
		CLCD_voidSetPosition(CLCD_ROW_2, 7);
		CLCD_voidSendString((u8*)"LEDS OFF");

	}
	else if (Mode == 'A')
	{
		CLCD_voidSetPosition(CLCD_ROW_2, 7);
		CLCD_voidSendString((u8*)"AUTO");

	}

}
