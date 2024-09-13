#include "stm32f4xx.h"
#include "AS7265X.h"


int main(void)
{
	bool works = begin();

	disableLED(AS7265x_LED_WHITE);
	while(1)
	{
		uint8_t red = getRawI();
		printf("Red: %d\n\r", red);
//		uint8_t hardware= getHardwareVersion();
//		printf("Works: %d\n\r", works);

	}
}
