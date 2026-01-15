#define F_CPU 8000000UL
#include <util/delay.h>
#include <string.h>

/* LIB */
#include "../LIBS/STD_TYPES.h"
#include "../LIBS/BIT_MATH.h"

/* MCAL */
#include "../MCAL/DIO/DIO_interface.h"
#include "../MCAL/GIE/GIE_interface.h"
#include "../MCAL/ADC/ADC_interface.h"
#include "../MCAL/TIMER/TIMER1_interface.h"
#include "../MCAL/UART/UART_interface.h"
#include "../MCAL/SPI/SPI_interface.h"

/* HAL */
#include "../HAL/CLCD/CLCD_interface.h"

/* ---------------- LOGIN DATA ---------------- */
#define PASS 1
#define FAIL 0

u8 Users[3][10] = {"SEIF#", "ABDO#", "YOSEF#"};
u8 Passw[3][10] = {"1319#", "1317#", "1399#"};

/* ---------------- PROTOTYPES ---------------- */
u8   Func_Login(void);
u8   CalcTempSensReading(void);
u8   UART_ReceiveCommand_NonBlocking(u8 *Buffer);
void Update_LCD_Display(u8 Temp, u8 Mode);

/* ================================================= */

int main(void)
{
	/* ---------- PIN CONFIG ---------- */
	/* SPI PIN CONFIG */
	DIO_voidSetPinDirection(DIO_PORTB, DIO_PIN5, DIO_OUTPUT); // MOSI
	DIO_voidSetPinDirection(DIO_PORTB, DIO_PIN6, DIO_INPUT);  // MISO
	DIO_voidSetPinDirection(DIO_PORTB, DIO_PIN7, DIO_OUTPUT); // SCK
	DIO_voidSetPinDirection(DIO_PORTB, DIO_PIN4, DIO_OUTPUT); // SS

	/* UART PIN CONFIG */
	DIO_voidSetPinDirection(DIO_PORTD, DIO_PIN0, DIO_INPUT);  // RX
	DIO_voidSetPinDirection(DIO_PORTD, DIO_PIN1, DIO_OUTPUT); // TX

	/* PERIPHERALS PIN CONFIG */
	DIO_voidSetPinDirection(DIO_PORTD, DIO_PIN2, DIO_OUTPUT); // FAN
	DIO_voidSetPinDirection(DIO_PORTD, DIO_PIN3, DIO_OUTPUT); // BUZZER
	DIO_voidSetPinDirection(DIO_PORTD, DIO_PIN5, DIO_OUTPUT); // SERVO (PWM)


	/* ---------- INITIALIZATION ---------- */
	UART_voidInit();
	CLCD_voidInit();
	ADC_voidInit();
	SPI_voidMasterInit();
	TIMER1_Init();
	TIMER1_SetICRValue(20000);
	GIE_voidEnable();
	TIMER1_SetCompareValue(1000);
	CLCD_voidSendString((u8*)"System Ready...");
	_delay_ms(1000);

	/* --- Security Layer --- */
	u8 LoginState = Func_Login();
	u8 SystemTemp;
	u8 UART_Command[20];
	u8 CommandReadyFlag = 0;
	u8 CurrentMode = 0; 
	// 0:Door, 1:Fan, 2:Light

	while(1)
	{
		if(LoginState == PASS)
		{
			SystemTemp = CalcTempSensReading();
			Update_LCD_Display(SystemTemp, CurrentMode);

			/* FIRE ALARM */
			if(SystemTemp > 35) {
				DIO_voidSetPinValue(DIO_PORTD, DIO_PIN3, DIO_HIGH);
			} else {
				DIO_voidSetPinValue(DIO_PORTD, DIO_PIN3, DIO_LOW);
			}

			/* RECEIVE COMMANDS */
			CommandReadyFlag = UART_ReceiveCommand_NonBlocking(UART_Command);

			if(CommandReadyFlag == 1)
			{
				/* DEBUG: Show what was received on LCD */
				CLCD_voidSetPosition(CLCD_ROW_1, 12);
				CLCD_voidSendString(UART_Command);

				/* MODE SWITCHING (Checking first character) */
				if(UART_Command[0] == '0')      CurrentMode = 0;
				else if(UART_Command[0] == '1') CurrentMode = 1;
				else if(UART_Command[0] == '2') CurrentMode = 2;

				/* LIGHT CONTROL VIA SPI */
				if(CurrentMode == 2) {
					if(strcmp((char*)UART_Command, "O") == 0)      SPI_u8TranciveInit('O'); // Manual ON
					else if(strcmp((char*)UART_Command, "C") == 0) SPI_u8TranciveInit('C'); // Manual OFF
					else if(strcmp((char*)UART_Command, "A") == 0) SPI_u8TranciveInit('A'); // AUTO MODE
				}
				/* EXECUTE ACTIONS (O for OPEN/ON, C for CLOSE/OFF) */
				else if(strcmp((char*)UART_Command, "O") == 0)
				{
					if(CurrentMode == 0){
						TIMER1_SetCompareValue(2000);
					}// Servo Open
					else if(CurrentMode == 1) DIO_voidSetPinValue(DIO_PORTD, DIO_PIN2, DIO_HIGH); // Fan ON
				}
				else if(strcmp((char*)UART_Command, "C") == 0)
				{
					if(CurrentMode == 0)      TIMER1_SetCompareValue(1000);  // Servo Close
					else if(CurrentMode == 1) DIO_voidSetPinValue(DIO_PORTD, DIO_PIN2, DIO_LOW);  // Fan OFF
				}
			}
		}
	}
}

/* ================================================= */

void Update_LCD_Display(u8 Temp, u8 Mode)
{
	CLCD_voidSetPosition(CLCD_ROW_1, 1);
	CLCD_voidSendString((u8*)"0:D 1:F 2:L");

	CLCD_voidSetPosition(CLCD_ROW_2, 1);
	CLCD_voidSendString((u8*)"T:");
	CLCD_voidSendData((Temp / 10) + '0');
	CLCD_voidSendData((Temp % 10) + '0');
	CLCD_voidSendString((u8*)"C M:");

	if(Mode == 0)      CLCD_voidSendString((u8*)"Door ");
	else if(Mode == 1) CLCD_voidSendString((u8*)"Fan  ");
	else if(Mode == 2) CLCD_voidSendString((u8*)"Light");
}

u8 UART_ReceiveCommand_NonBlocking(u8 *Buffer)
{
	static u8 i = 0;
	u8 ch;
	// Check UCSRA RXC Flag (Direct Access for reliability)
	if ( ( (*(volatile u8*)0x2B) & (1<<7) ) )
	{
		ch = UART_u8Receive();
		if(ch == '\r' || ch == '\n') return 0; // Ignore newlines

		if(ch == '#') {
			Buffer[i] = '\0';
			i = 0;
			return 1;
		} else {
			if(i < 19) Buffer[i++] = ch;
		}
	}
	return 0;
}

#define NUM_SAMPLES 5

u8 CalcTempSensReading(void)
{
	static u16 samples[NUM_SAMPLES] = {0};
	static u8 index = 0;
	static u32 sum = 0;
	u16 new_sample = ADC_u16ConvertedValue(ADC_CHANNEL_0);
	u32 mv = ((u32)new_sample * 5000UL) / 1024UL;
	u8 tempC = (u8)(mv / 10);

	sum -= samples[index];
	samples[index] = tempC;
	sum += samples[index];

	index = (index + 1) % NUM_SAMPLES;

	return (u8)(sum / NUM_SAMPLES);
}




u8 Func_Login(void)
{
	u8 UserBuf[15], PassBuf[15], i, j, tries = 0;
	while(tries < 3)
	{
		CLCD_voidSendCommand(0x01); // Clear
		CLCD_voidSendString((u8*)"User:");
		i = 0;
		do {
			UserBuf[i] = UART_u8Receive();
			i++;
		} while(UserBuf[i-1] != '#');
		UserBuf[i] = '\0';

		for(j = 0; j < 3; j++) {
			if(strcmp((char*)UserBuf, (char*)Users[j]) == 0) {
				CLCD_voidSendString((u8*)" Pass:");
				i = 0;
				do {
					PassBuf[i] = UART_u8Receive();
					i++;
				} while(PassBuf[i-1] != '#');
				PassBuf[i] = '\0';

				if(strcmp((char*)PassBuf, (char*)Passw[j]) == 0) {
					CLCD_voidSendCommand(0x01);
					CLCD_voidSendString((u8*)"ACCESS GRANTED");
					TIMER1_SetCompareValue(2000);
					_delay_ms(4000);
					TIMER1_SetCompareValue(1000);
					return PASS;
				}
			}
		}
		tries++;
		CLCD_voidSendString((u8*)" WRONG!");
		_delay_ms(1000);
	}
	return FAIL;
}
