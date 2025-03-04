/* Added averaging */
//#include "stm32f4xx.h"
//#include "AS7421.h"
//#include <inttypes.h>
//#include <limits.h>
//#include "mux.h"
//
//
//#define AVERAGE_COUNT       30
//
//int main(void)
//{
//	I2C1_Init();
//
//	/* Mux *
//	 * Note: calling enableChannel closes all the mux outputs before opening the specified channel
//	 * Switching between channels puts previously ON channel in idle mode (LED still on but not measuring)
//	 * Sensor state of previously ON channel resets to sleep mode with power on reset (i.e. power off then power on)*/
//	enableChannel(CHANNEL_1);
//
//	/* Sensor */
//	// Containers to receive channel data
//	uint16_t channel_data[CHANNELSIZE];
//	uint16_t temp_data[CHANNELSIZE];
//
//	uint32_t sum_data[CHANNELSIZE] = {0};
//	float avg_data[CHANNELSIZE] = {0};
//
//	startup();
//	delayMillis(2000);
//	startMeasurements(true);
//
//	int count = 0;
//	while (measurementActive())
//	{
//		performMeasurements(channel_data, temp_data);
//		printf("%i\n\r", -1);
//		printf("%i\n\r", count);
//
//        // Accumulate the readings
//        for (int i = 0; i < CHANNELSIZE; i++)
//        {
//            sum_data[i] += channel_data[i];
//        }
//
//
//        if (count >= AVERAGE_COUNT)
//		{
//			for (int i = 0; i < CHANNELSIZE; i++)
//			{
//				avg_data[i] = (float)sum_data[i] / AVERAGE_COUNT;
//				sum_data[i] = 0; //Reset sum for next averaging window
//			}
//
//            for (int i = 0; i < CHANNELSIZE; i++)
//            {
//            	printf("%.3f\n\r", avg_data[i]);
//            }
//
//            count = 0;
//		}
//
//        count++;
//
//	}
//	stopMeasurements();
//	sleep();
//
//	return 0;
//}


/* OLD MAIN */
#include "stm32f4xx.h"
#include "AS7421.h"
#include <inttypes.h>
#include <limits.h>
#include "mux.h"


#define NUM_CHANNELS        64

int main(void)
{
    I2C1_Init();

    /* Mux *
     * Note: calling enableChannel closes all the mux outputs before opening the specified channel
     * Switching between channels puts previously ON channel in idle mode (LED still on but not measuring)
     * Sensor state of previously ON channel resets to sleep mode with power on reset (i.e. power off then power on)*/
    enableChannel(CHANNEL_1);

    /* Sensor */
    // Containers to receive channel data
    uint16_t channel_data[CHANNELSIZE];
    uint16_t temp_data[CHANNELSIZE];

    startup();
    delayMillis(2000);
    startMeasurements(true);

//    unsigned long start = getMillis();
//    unsigned long duration = 120000; // aka 2mins
    int count = 0;
    while (measurementActive())
    {
        performMeasurements(channel_data, temp_data);
        printf("%i\n\r", -1);
        printf("%i\n\r", count);
        for (int i = 0; i < NUM_CHANNELS; i++)
        {
//            printf("Channel %i: %f\n\r", i+1, channel_data[i]);
            printf("%d\n\r", channel_data[i]);
        }
        count++;
        if(count ==    INT_MAX){
            count = 0;
        }
        //unsigned long end = getMillis();
        // If in continuous mode, specify duration of test
        //if (end - start > duration)
        //{
        //    break;
        //}
    }
    stopMeasurements();
    sleep();

    return 0;
}
