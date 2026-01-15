
#ifndef MCAL_EXTI_EXTI_PRIVATE_H_
#define MCAL_EXTI_EXTI_PRIVATE_H_

#define MCUCR		*((volatile u8*)0x55)
#define MCUCSR		*((volatile u8*)0x54)
#define GICR		*((volatile u8*)0x5B)
#define GIFR		*((volatile u8*)0x5A)

#define INT0_BIT	6
#define INT1_BIT	7
#define INT2_BIT	5


#endif /* MCAL_EXTI_EXTI_PRIVATE_H_ */
