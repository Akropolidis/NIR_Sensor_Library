#include "gpio.h"


#define GPIOAEN			(1U<<0)
#define GPIOCEN			(1U<<2)

#define PIN5			(1U<<5)
#define PIN13			(1U<<13)

#define LED_PIN			PIN5
#define BTN_PIN			PIN13

void GPIO_Init(void)
{
	/*Enable clock access to GPIOA and GPIOC*/
	RCC->AHB1ENR |= GPIOAEN;
	RCC->AHB1ENR |= GPIOCEN;

	/*Set PA5 as output pin*/
	GPIOA->MODER |= (1U<<10);
	GPIOA->MODER &=~ (1U<<11);

	/*Set PC13 as input pin*/
	GPIOC->MODER &=~ (1U<<26);
	GPIOC->MODER &=~ (1U<<27);
}

bool UserBtn_Control(void)
{
	/*Check if BTN is pressed*/
	if (!(GPIOC->IDR & BTN_PIN))
	{
		/*Turn on LED*/
		GPIOA->BSRR = LED_PIN;
		return true;
	}
	else
	{
		/*Turn off LED*/
		GPIOA->BSRR = (1U<<21);
		return false;
	}
}
