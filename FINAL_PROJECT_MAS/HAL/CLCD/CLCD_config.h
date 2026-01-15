/*
 * CLCD_config.h
 *
 *  Created on: Aug 30, 2025
 *      Author: mohamed.khaled
 */

#ifndef HAL_CLCD_CLCD_CONFIG_H_
#define HAL_CLCD_CLCD_CONFIG_H_

/*
 * options:
 * 			1. DIO_PORTA
 * 			2. DIO_PORTB
 * 			3. DIO_PORTC
 * 			4. DIO_PORTD
*/

#define CLCD_DATA_PORT			DIO_PORTC

#define CLCD_CTRL_PORT			DIO_PORTA

/*
 * options:
 * 			1. DIO_PIN0
 * 			2. DIO_PIN1
 *
 * 			8. DIO_PIN7
 * */
#define CLCD_RS_PIN			DIO_PIN1
#define CLCD_RW_PIN			DIO_PIN2
#define CLCD_EN_PIN			DIO_PIN3



#endif /* HAL_CLCD_CLCD_CONFIG_H_ */
