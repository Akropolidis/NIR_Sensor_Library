#include "stm32f4xx.h"
#include "systick.h"


#define SYSTICK_LOAD_VAL		16000
#define CTRL_ENABLE				(1U<<0)
#define CTRL_TICKINT			(1U<<1)
#define CTRL_CLKSRC				(1U<<2)
#define CTRL_COUNTFLAG			(1U<<16)

volatile uint32_t msTicks = 0;

void SysTick_Init(void)
{
    // Reload with the number of clocks per millisecond
    SysTick->LOAD = SYSTICK_LOAD_VAL - 1;  // Set reload register (off by one)

    // Set the priority of the SysTick interrupt
    NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);  // Set lowest priority

    // Reset the SysTick counter value
    SysTick->VAL = 0;

    // Select processor clock as SysTick clock source
    SysTick->CTRL |= CTRL_CLKSRC;

    // Enable SysTick interrupt
    SysTick->CTRL |= CTRL_TICKINT;

    // Enable SysTick timer
    SysTick->CTRL |= CTRL_ENABLE;
}

// SysTick Handler - Called every 1ms
void SysTick_Handler(void)
{
	msTicks++;
}

// Function to get the current time in milliseconds
uint32_t getMillis(void)
{
	return msTicks;
}

void delayMillis(uint32_t delay)
{
	uint32_t startTick = getMillis();
	while ((getMillis() - startTick) < delay){}
}


//void systickDelayMs(int delay)
//{
//	/*Reload with number of clocks per millisecond*/
//	SysTick->LOAD = SYSTICK_LOAD_VAL;
//
//	/*Clear systick current value register*/
//	SysTick->VAL = 0;
//
//	/*Enable Systick and select internal clk src*/
//	SysTick->CTRL = CTRL_ENABLE | CTRL_CLKSRC;
//
//	for (int i=0; i<delay; i++)
//	{
//		/*Wait until the count flag is set*/
//		while((SysTick->CTRL & CTRL_COUNTFLAG) == 0){}
//	}
//	SysTick->CTRL = 0;
//}
