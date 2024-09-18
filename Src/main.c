#include "stm32f4xx.h"
#include "AS7265X.h"


#define NUM_CHANNELS	18


void log_data_to_csv(const char* filename, uint32_t* data, int num_channels);


int main(void)
{

	uint16_t channel_data[NUM_CHANNELS];
	float channel_data_cal[NUM_CHANNELS];

//	const char* filename = "C:	C:\\Users\\adeya\\Downloads\\as7265x_log.csv";
//	FILE* log = fopen(filename, "w"); //Opening new file and enabling write
//	if (log != NULL)
//	{
//		fprintf(log,"Channel_1,Channel_2,Channel_3,Channel_4,Channel_5,Channel_6,Channel_7,Channel_8,Channel_9,Channel_10,Channel_11,Channel_12,Channel_13,Channel_14,Channel_15,Channel_16,Channel_17,Channel_18\n");
//		fclose(log);
//	}
//	if (log == NULL)
//	{
//		printf("errno: %d, message: %s\n", errno, strerror(errno));
//	}



	bool works = begin();

	while(1)
	{
		enableLED(AS7265x_LED_UV);
		delayMillis(3000);
		channel_data[0] = getRawA();
		channel_data[1] = getRawB();
		channel_data[2] = getRawC();
		channel_data[3] = getRawD();
		channel_data[4] = getRawE();
		channel_data[5] = getRawF();
		disableLED(AS7265x_LED_UV);
		delayMillis(3000);

		enableLED(AS7265x_LED_WHITE);
		delayMillis(3000);
		channel_data[6] = getRawG();
		channel_data[7] = getRawH();
		channel_data[9] = getRawI();
		channel_data[11] = getRawJ();
		channel_data[16] = getRawL();
		channel_data[17] = getRawL();
		disableLED(AS7265x_LED_WHITE);
		delayMillis(3000);

		enableLED(AS7265x_LED_IR);
		delayMillis(3000);
		channel_data[8] = getRawR();
		channel_data[10] = getRawS();
		channel_data[12] = getRawT();
		channel_data[13] = getRawU();
		channel_data[14] = getRawV();
		channel_data[15] = getRawW();
		disableLED(AS7265x_LED_IR);
		delayMillis(3000);


		printf("\nRaw Data Channels \n\r");
		for (int i = 0; i < NUM_CHANNELS; i++)
		{
//			printf("Channel %i: %u\n\r", i+1, channel_data[i]);
			printf("%u\n\r", channel_data[i]);
		}

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
		channel_data_cal[16] = getCalibratedL();
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

//		// Log the data to CSV
//		log_data_to_csv(filename, channel_data, NUM_CHANNELS);

	}
	return 0;
}


void log_data_to_csv(const char* filename, uint32_t* data, int num_channels)
{
	FILE* file = fopen(filename, "a"); //Writes data at the end of the file. Creates file if it does not exist
	if (file == NULL)
	{
		printf("Error opening file");
		return;
	}

	//Write data to CSV
	for (int i = 0; i < num_channels; i++)
	{
		fprintf(file, "%u", data[i]);
		if (i < num_channels - 1) // Comma is not added to last number
		{
			fprintf(file, ","); //Comma Separated Values
		}
	}
	fprintf(file, "\n");

	fclose(file);

}
