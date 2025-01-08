#include "stm32f4xx.h"
#include "AS7421.h"


#define NUM_CHANNELS		64

int main(void)
{
	// Containers to receive channel data
	uint16_t channel_data[CHANNELSIZE];
	uint16_t temp_data[CHANNELSIZE];

	startup();
	delayMillis(2000);
	startMeasurements(true);
	unsigned long start = getMillis();
	unsigned long duration = 120000; // aka 2mins
	while (measurementActive())
	{
		performMeasurements(channel_data, temp_data);

//		printf("Measurement %i: \n\r", count);
		for (int i = 0; i < NUM_CHANNELS; i++)
		{
//			printf("Channel %i: %f\n\r", i+1, channel_data[i]);
			printf("%d\n\r", channel_data[i]);
		}

		unsigned long end = getMillis();
		// If in continuous mode, specify duration of test
		if (end - start > duration)
		{
			break;
		}
	}
	stopMeasurements();
	sleep();

	return 0;
}
