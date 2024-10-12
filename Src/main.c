#include "stm32f4xx.h"
#include "AS7265X.h"


#define NUM_CHANNELS	18


int main(void)
{

	uint16_t channel_data[NUM_CHANNELS];
	float channel_data_cal[NUM_CHANNELS];

	bool works = initialize();

	while(1)
	{
		// Calibrated Channels
		enableLED(AS7265x_LED_UV);
		delayMillis(3000);
		channel_data_cal[0] = getCalibratedA();
		channel_data_cal[1] = getCalibratedB();
		channel_data_cal[2] = getCalibratedC();
		channel_data_cal[3] = getCalibratedD();
		channel_data_cal[4] = getCalibratedE();
		channel_data_cal[5] = getCalibratedF();
		disableLED(AS7265x_LED_UV);
		delayMillis(3000);

		enableLED(AS7265x_LED_WHITE);
		delayMillis(3000);
		channel_data_cal[6] = getCalibratedG();
		channel_data_cal[7] = getCalibratedH();
		channel_data_cal[9] = getCalibratedI();
		channel_data_cal[11] = getCalibratedJ();
		channel_data_cal[16] = getCalibratedK();
		channel_data_cal[17] = getCalibratedL();
		disableLED(AS7265x_LED_WHITE);
		delayMillis(3000);

		enableLED(AS7265x_LED_IR);
		delayMillis(3000);
		channel_data_cal[8] = getCalibratedR();
		channel_data_cal[10] = getCalibratedS();
		channel_data_cal[12] = getCalibratedT();
		channel_data_cal[13] = getCalibratedU();
		channel_data_cal[14] = getCalibratedV();
		channel_data_cal[15] = getCalibratedW();
		disableLED(AS7265x_LED_IR);
		delayMillis(3000);

		printf("\nCalibrated Data Channels \n\r");
		for (int i = 0; i < NUM_CHANNELS; i++)
		{
//			printf("Channel %i: %f\n\r", i+1, channel_data_cal[i]);
			printf("%f\n\r", channel_data_cal[i]);
		}

		break;

	}
	return 0;
}
