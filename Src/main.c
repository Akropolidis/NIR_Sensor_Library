#include "stm32f4xx.h"
#include "AS7265X.h"


int main(void)
{
	bool works = 1;
	works = begin();
	while(1)
	{
		enableLED(AS7265x_LED_UV);
		delayMillis(1000);
		disableLED(AS7265x_LED_UV);
		delayMillis(1000);
//		uint8_t hardware= getHardwareVersion();
//		printf("works: %d\n\r", works);
//		printf("Hardware Version: %d\n\r", hardware);

	}
}
