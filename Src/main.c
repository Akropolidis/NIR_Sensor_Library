#include "stm32f4xx.h"
#include "AS7421.h"


#define NUM_CHANNELS	18


int main(void)
{

	uint16_t channel_data[CHANNELSIZE];
	uint16_t temp_data[CHANNELSIZE];


	startup();
	performMeasurements(channel_data, temp_data);
	sleep();

//	while(1)
//	{
//
//	}

	return 0;
}
