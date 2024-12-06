#include "stm32f4xx.h"
#include "AS7421.h"


#define NUM_CHANNELS		64

int main(void)
{
	// Containers to receive channel data
	uint16_t channel_data[CHANNELSIZE];
	uint16_t temp_data[CHANNELSIZE];


	startup();
	startMeasurements(true);
	uint8_t count = 0;
	uint32_t cycles = (20 * F_CLKMOD) - 1;
	while (1)
	{

		unsigned long startTime = getMillis();
		while (getMeasurementStatus(ADATA) == 0){} //End of measurement, new measurement data can be read if true

		unsigned long endTime = getMillis();
		printf("Time to get data: %ld \n\r", endTime - startTime);

		getAllSpectralData(channel_data); //Reading spectral data channels and passing organized values into arrSpectra
		getAllTemperatureData(temp_data); //Reading temperatures of integration cycles A to D

		printf("\nMeasurement %i: \n\r", count);
		for (int i = 0; i < NUM_CHANNELS; i++)
		{
//			printf("Channel %i: %f\n\r", i+1, channel_data[i]);
			printf("%d\n\r", channel_data[i]);
		}

		count++;
		if (count < 5)
		{
			startMeasurements(true);
		}
		else
		{
			break;
		}
	}
	stopMeasurements();
	sleep();

	return 0;
}
