#include "AS7265X.h"


uint16_t maxWaitTime = STARTUP_DELAY; //Based on integration cycles, initial delay of 10ms before checking for sensor status

static uint8_t readRegister(uint8_t addr);
static void writeRegister(uint8_t addr, uint8_t val);

static uint8_t virtualReadRegister(uint8_t virtualAddr);
static void virtualWriteRegister(uint8_t virtualAddr, uint8_t dataToWrite);

static void selectDevice(uint8_t device); //Change between the x51, x52, or x53 for data and settings

static float convertBytesToFloat(uint32_t myLong);
static uint16_t getChannel(uint8_t channelRegister, uint8_t device);
static float getCalibratedValue(uint8_t calAddress, uint8_t device);


//Initializes the sensor with basic settings
//Returns false if sensor is not detected
bool begin()
{
	SysTick_Init();
	uart2_rxtx_init();
	I2C1_Init();
	fpu_enable();

	if (isConnected() == false)
	{
		return false; //Check for sensor presence
	}

	uint8_t value = virtualReadRegister(AS7265X_DEV_SELECT_CONTROL);
	if ((value & 0b00110000) == 0) //Check for presence of first and second slave devices
	{
		return false;
	}

	setLEDCurrent(AS7265X_LED_CURRENT_LIMIT_12_5MA, AS7265x_LED_WHITE);
	setLEDCurrent(AS7265X_LED_CURRENT_LIMIT_12_5MA, AS7265x_LED_IR);
	setLEDCurrent(AS7265X_LED_CURRENT_LIMIT_12_5MA, AS7265x_LED_UV);

	disableLED(AS7265x_LED_WHITE); //Turn off bulb to avoid heating sensor
	disableLED(AS7265x_LED_IR);
	disableLED(AS7265x_LED_UV);

	setIndicatorCurrent(AS7265X_INDICATOR_CURRENT_LIMIT_2MA);
	enableIndicator();

	setIntegrationCycles(49); //(49 + 1) * 2.78ms = 139ms. 0 to 255 is valid.
	//If you use Mode 2 or 3 (all the colors) then integration time is double. 139*2 = 278ms between readings.

	setGain(AS7265X_GAIN_64X);

	setMeasurementMode(AS7265X_MEASUREMENT_MODE_6CHAN_ONE_SHOT);

	enableInterrupt();

	return true;
}

//Returns TRUE if the device sends an ACK indicating it is connected
bool isConnected()
{
	uint32_t timeout = 1000; //Timeout to connect set to 1000ms
	unsigned long startTime = getMillis();

	while ((getMillis() - startTime) < timeout)
	{
		volatile int tmp;

		/* Wait until bus not busy */
		while (I2C1->SR2 & (SR2_BUSY)){}

		/* Generate start condition */
		I2C1->CR1 |= CR1_START;

		/* Wait until start condition flag is set */
		while (!(I2C1->SR1 & (SR1_SB))){}

		/* Transmit slave address + Write (0) */
		I2C1->DR = AS7265X_WRITE_ADDR;

		/* Wait until address flag is set */
		while (!(I2C1->SR1 & (SR1_ADDR))){}

		/* Clear address flag by reading SR2 register */
		tmp = I2C1->SR2;

		if (I2C1->SR1 & SR1_AF)
		{
			//No ACK received, address not acknowledged
			I2C1->CR1 |= CR1_STOP;  // Send STOP condition
			continue;
		}

		//Address acknowledged, device is connected
		I2C1->CR1 |= CR1_STOP; // Send STOP condition
		return true;
	}
	//Timeout to connect has expired, hence device is not connected
	return false;
}

void fpu_enable()
{
	/*Enable Floating Point Unit: Enable CP10 and CP11 full access*/
	SCB->CPACR |= (1U<<20);
	SCB->CPACR |= (1U<<21);
	SCB->CPACR |= (1U<<22);
	SCB->CPACR |= (1U<<23);
}

//Reads from a given location from the AS726x
static uint8_t readRegister(uint8_t addr)
{
	char data = 0;
	I2C1_byteRead(AS7265X_ADDR, addr, &data);
	return data;
}

//Write a value to a given location on the AS726x
static void writeRegister(uint8_t addr, uint8_t val)
{
	uint8_t bufferSize = 1;
	char data[bufferSize];

	data[0] = val;
	I2C1_burstWrite(AS7265X_ADDR, addr, bufferSize, data);
}

//Read from a virtual register on the AS7265x
static uint8_t virtualReadRegister(uint8_t virtualAddr)
{
	volatile uint8_t status, data;

	//Do a preliminary check of the read register
	status = readRegister(AS7265X_STATUS_REG);
	if((status & AS7265X_RX_VALID) != 0) //Data byte available in READ register
	{
		readRegister(AS7265X_READ_REG); //Read the byte but do nothing
	}

	//Wait for WRITE flag to clear
	unsigned long startTime = getMillis();
	while(1)
	{
		if((getMillis() - startTime) > maxWaitTime)
		{
			printf("Sensor failed to respond \n\r");
			return 0;
		}
		//Read slave I2C status to see if the read register is ready
		status = readRegister(AS7265X_STATUS_REG);
		if((status & AS7265X_TX_VALID) == 0) //New data may be written to WRITE register
		{
			break;
		}
		delayMillis(AS7265X_POLLING_DELAY); //Delay for 5 ms before checking for virtual register changes
	}

	//Send the virtual register address (disabling bit 7 to indicate a read).
	writeRegister(AS7265X_WRITE_REG, virtualAddr);

	//Wait for READ flag to be set
	startTime = getMillis();
	while(1)
	{
		if((getMillis() - startTime) > maxWaitTime)
		{
			printf("Sensor failed to respond \n\r");
			return 0;
		}
		//Read slave I2C status to see if the read register is ready
		status = readRegister(AS7265X_STATUS_REG);
		if((status & AS7265X_RX_VALID) != 0) //Data byte available in READ register
		{
			break;
		}
		delayMillis(AS7265X_POLLING_DELAY); //Delay for 5 ms before checking for virtual register changes
	}

	data = readRegister(AS7265X_READ_REG);
	return data;
}

//Write to a virtual register in the AS7265x
static void virtualWriteRegister(uint8_t virtualAddr, uint8_t dataToWrite)
{
	volatile uint8_t status;

	//Wait for WRITE register to be empty
	unsigned long startTime = getMillis();
	while(1)
	{
		if((getMillis() - startTime) > maxWaitTime)
		{
			printf("Sensor failed to respond \n\r");
			return;
		}
		//Read slave I2C status to see if the write register is ready
		status = readRegister(AS7265X_STATUS_REG);
		if((status & AS7265X_TX_VALID) == 0) //New data may be written to WRITE register
		{
			break;
		}
		delayMillis(AS7265X_POLLING_DELAY); //Delay for 5 ms before checking for virtual register changes
	}

	//Send the virtual register address (enabling bit 7 to indicate a write).
	writeRegister(AS7265X_WRITE_REG, (virtualAddr | 1 << 7));

	//Wait for WRITE register to be empty
	startTime = getMillis();
	while(1)
	{
		if((getMillis() - startTime) > maxWaitTime)
		{
			printf("Sensor failed to respond \n\r");
			return;
		}
		//Read slave I2C status to see if the write register is ready
		status = readRegister(AS7265X_STATUS_REG);
		if((status & AS7265X_TX_VALID) == 0) //New data may be written to WRITE register
		{
			break;
		}
		delayMillis(AS7265X_POLLING_DELAY); //Delay for 5 ms before checking for virtual register changes
	}

	//Send the data to complete the operation
	writeRegister(AS7265X_WRITE_REG, dataToWrite);
}

//As we read various registers we have to point at the master or first/second slave
static void selectDevice(uint8_t device)
{
	virtualWriteRegister(AS7265X_DEV_SELECT_CONTROL, device);
}

//Given 4 bytes (size of float and uint32_t) and returns the floating point value
float convertBytesToFloat(uint32_t myLong)
{
  float myFloat;
  memcpy(&myFloat, &myLong, 4); //Copy bytes into a float
  return myFloat;
}

//Get the 16-bit raw values stored in the high and low registers of each channel
static uint16_t getChannel(uint8_t channelRegister, uint8_t device)
{
	selectDevice(device);

	uint16_t colorData = virtualReadRegister(channelRegister) << 8; //XXXXXXXX-00000000 High uint8_t
	colorData |= virtualReadRegister(channelRegister + 1); //Low uint8_t
	return colorData;
}

//Given an address, read four consecutive bytes and return the floating point calibrated value
static float getCalibratedValue(uint8_t calAddress, uint8_t device)
{
	selectDevice(device);

	uint8_t chan0, chan1, chan2, chan3;
	chan0 = virtualReadRegister(calAddress + 0);
	chan1 = virtualReadRegister(calAddress + 1);
	chan2 = virtualReadRegister(calAddress + 2);
	chan3 = virtualReadRegister(calAddress + 3);

	//Channel calibrated values are stored big-endian
	uint32_t calBytes = 0;
	calBytes |= ((uint32_t)chan0 << (8 * 3)); //bits 24-31
	calBytes |= ((uint32_t)chan1 << (8 * 2)); //bits 16-23
	calBytes |= ((uint32_t)chan2 << (8 * 1)); //bits 8-15
	calBytes |= ((uint32_t)chan3 << (8 * 0)); //bits 0-7

	return convertBytesToFloat(calBytes);

/*1. 00000000 00000000 00000000 WWWWWWWW
a. WWWWWWWW 00000000 00000000 00000000
b. calBytes = WWWWWWWW 00000000 00000000 00000000

2. 00000000 00000000 00000000 XXXXXXXX
a. 00000000 XXXXXXXX 00000000 00000000
b. calBytes = WWWWWWWW XXXXXXXX 00000000 00000000

3. 00000000 00000000 00000000 YYYYYYYY
a. 00000000 00000000 YYYYYYYY 00000000
b. calBytes = WWWWWWWW XXXXXXXX YYYYYYYY 00000000

4. 00000000 00000000 00000000 ZZZZZZZZ
a. 00000000 00000000 00000000 ZZZZZZZZ
b. calBytes = WWWWWWWW XXXXXXXX YYYYYYYY ZZZZZZZZ
*/
}

uint8_t getDeviceType()
{
	return (virtualReadRegister(AS7265X_HW_VERSION_HIGH));
}

uint8_t getHardwareVersion()
{
	return (virtualReadRegister(AS7265X_HW_VERSION_LOW));
}

uint8_t getMajorFirmwareVersion()
{
	virtualWriteRegister(AS7265X_FW_VERSION_HIGH, 0x01); //Set to 0x01 for Major
	virtualWriteRegister(AS7265X_FW_VERSION_LOW, 0x01);  //Set to 0x01 for Major

	return (virtualReadRegister(AS7265X_FW_VERSION_HIGH));
}

uint8_t getPatchFirmwareVersion()
{
	virtualWriteRegister(AS7265X_FW_VERSION_HIGH, 0x02); //Set to 0x02 for Patch
	virtualWriteRegister(AS7265X_FW_VERSION_LOW, 0x02);  //Set to 0x02 for Patch

	return (virtualReadRegister(AS7265X_FW_VERSION_HIGH));
}

uint8_t getBuildFirmwareVersion()
{
	virtualWriteRegister(AS7265X_FW_VERSION_HIGH, 0x03); //Set to 0x03 for Build
	virtualWriteRegister(AS7265X_FW_VERSION_LOW, 0x03);  //Set to 0x03 for Build

	return (virtualReadRegister(AS7265X_FW_VERSION_HIGH));
}

//Returns the temperature of a given device in C
uint8_t getTemperature(uint8_t deviceNumber)
{
	selectDevice(deviceNumber);
	return (virtualReadRegister(AS7265X_DEVICE_TEMP));
}

//Get average temperature in C of all three sensors
float getTemperatureAverage()
{
	float average = 0;

	for (uint8_t i = 0; i < 3; i++)
	{
		average += getTemperature(i);
	}

	average = (average / 3);
	return average;
}

//Enable the on-board indicator LED on the NIR master device, Blue status LED
void enableIndicator()
{
	selectDevice(AS72651_NIR);

	uint8_t value = virtualReadRegister(AS7265X_LED_CONFIG); //Read existing state
	value |= (1U << 0); //Set ENABLE LED_INT bit (bit 0)
	virtualWriteRegister(AS7265X_LED_CONFIG, value); //Write value to LED config register to enable the LED indicator
}

//Disable the on-board indicator LED on the NIR master device, Blue status LED
void disableIndicator()
{
	selectDevice(AS72651_NIR);

	uint8_t value = virtualReadRegister(AS7265X_LED_CONFIG); //Read existing state
	value &= ~(1U << 0); //Reset ENABLE LED_INT bit (bit 0)
	virtualWriteRegister(AS7265X_LED_CONFIG, value); //Write value to LED config register to disable the LED indicator
}

//Enable the LED or bulb on a given device
void enableLED(uint8_t device)
{
	selectDevice(device);

	uint8_t value = virtualReadRegister(AS7265X_LED_CONFIG); //Read existing state
	value |= (1U << 3); //Set ENABLE LED_DRV bit (bit 3)
	virtualWriteRegister(AS7265X_LED_CONFIG, value); //Write value to LED config register to enable the LED driver
}

//Disable the LED or bulb on a given device
void disableLED(uint8_t device)
{
	selectDevice(device);

	uint8_t value = virtualReadRegister(AS7265X_LED_CONFIG); //Read existing state
	value &= ~(1U << 3); //Reset ENABLE LED_DRV bit (bit 3)
	virtualWriteRegister(AS7265X_LED_CONFIG, value); //Write value to LED config register to disable the LED driver
}

//Set the gain value
//Gain 0: 1x
//Gain 1: 3.7x (Default)
//Gain 2: 16x
//Gain 3: 64x
void setGain(uint8_t gain)
{
	if (gain > AS7265X_GAIN_64X)
	{
		gain = AS7265X_GAIN_64X;
	}
	uint8_t value = virtualReadRegister(AS7265X_CONFIG); //Read existing state
	value &= 0b11001111; //Clear GAIN bits
	value |= (gain << 4); //Set GAIN bits with user's choice

	virtualWriteRegister(AS7265X_CONFIG, value); //Write value to config register
}

//Set the measurement mode
//Mode 0: 4 channels
//Mode 1: 4 channels
//Mode 2: All 6 channels (Default)
//Mode 3: One shot operation of mode 2
void setMeasurementMode(uint8_t mode)
{
	if (mode > AS7265X_MEASUREMENT_MODE_6CHAN_ONE_SHOT)
	{
		mode = AS7265X_MEASUREMENT_MODE_6CHAN_ONE_SHOT; //Limit mode to 2 bits
	}
	uint8_t value = virtualReadRegister(AS7265X_CONFIG); //Read existing state
	value &= 0b11110011; //Clear BANK bits
	value |= (mode << 2); //Set BANK bits with user's choice

	virtualWriteRegister(AS7265X_CONFIG, value); //Write value to config register
}

//Set the Integration cycles with a byte from 0 - 255 to set the sensitivity
//Every 2.78ms of integration increases the resolution of the ADC by 2^10 = 1024 counts
//Longer integration time means a more accurate measurement
//16-bit ADC so full sensitivity scale is clamped at 2^16 = 65536
void setIntegrationCycles(uint8_t cycleValue)
{
	if (cycleValue > 255)
	{
		cycleValue = 255; //Limit cycleValue to a byte 2^8 (0-255)
	}
	maxWaitTime = (int)((cycleValue + 1) * 2.78 * 1.5); //Wait for integration time + 50%

	virtualWriteRegister(AS7265X_INTEGRATION_TIME, cycleValue);
}

//Returns true when data is available to be read by checking config register
bool dataAvailable()
{
	uint8_t value = virtualReadRegister(AS7265X_CONFIG);
	return (value & (1U<<1)); //Bit 1 is DATA_RDY bit
}

//Tells IC to take all channel measurements and polls for data ready flag
void takeMeasurements()
{
	setMeasurementMode(AS7265X_MEASUREMENT_MODE_6CHAN_ONE_SHOT);

	//Wait for data to be ready
	unsigned long startTime = getMillis();
	while (dataAvailable() == false)
	{
		if ((getMillis() - startTime ) > maxWaitTime)
		{
			printf("Sensor failed to respond \n\r");
			return;
		}
		delayMillis(AS7265X_POLLING_DELAY);
	}
	//Readings can now be obtained, either Calibrated or RAW
}

//Turns on all LEDs, takes measurements of all channels, and turns off all LEDs
void takeMeasurementsWithLED()
{
	enableLED(AS7265x_LED_WHITE);
	enableLED(AS7265x_LED_IR);
	enableLED(AS7265x_LED_UV);

	takeMeasurements();

	disableLED(AS7265x_LED_WHITE);
	disableLED(AS7265x_LED_IR);
	disableLED(AS7265x_LED_UV);
}

//Set the current limit of chosen LED
//Current 0: 12.5mA (Default)
//Current 1: 25mA
//Current 2: 50mA
//Current 3: 100mA
void setLEDCurrent(uint8_t current, uint8_t device)
{
	selectDevice(device);

	if (current > AS7265X_LED_CURRENT_LIMIT_100MA)
	{
		current = AS7265X_LED_CURRENT_LIMIT_100MA;
	}
	uint8_t value = virtualReadRegister(AS7265X_LED_CONFIG); //Read existing state
	value &= 0b11001111; //Bitwise ANDing to reset LED current limit bits (bit5:4)
	value |= (current << 4); //Set LED current limit

	virtualWriteRegister(AS7265X_LED_CONFIG, value); //Write value to LED config register to set the LED current limit
}

//Set the current limit of on-board LED indicator
//Current 0: 1mA
//Current 1: 2mA (Default)
//Current 2: 4mA
//Current 3: 8mA
void setIndicatorCurrent(uint8_t current)
{
	selectDevice(AS72651_NIR);

	if (current > AS7265X_INDICATOR_CURRENT_LIMIT_8MA)
	{
		current = AS7265X_INDICATOR_CURRENT_LIMIT_8MA;
	}
	uint8_t value = virtualReadRegister(AS7265X_LED_CONFIG); //Read existing state
	value &= 0b11111001; //Bitwise ANDing to reset indicator current limit bits (bit2:1)
	value |= (current << 1); //Set indicator current limit

	virtualWriteRegister(AS7265X_LED_CONFIG, value); //Write value to LED config register to set the LED indicator current limit
}

//Enable interrupt pin
void enableInterrupt()
{

	uint8_t value = virtualReadRegister(AS7265X_CONFIG); //Read existing state
	value |= (1U << 6); //Set INT bit (bit 6)
	virtualWriteRegister(AS7265X_CONFIG, value); //Write value to config register to enable the interrupt pin
}

//Disable interrupt pin
void disableInterrupt()
{

	uint8_t value = virtualReadRegister(AS7265X_CONFIG); //Read existing state
	value &= ~(1U << 6); //Set INT bit (bit 6)
	virtualWriteRegister(AS7265X_CONFIG, value); //Write value to config register to disable the interrupt pin
}

//Does a soft reset, wait at least 1000ms
void softReset()
{
	uint8_t value = virtualReadRegister(AS7265X_CONFIG); //Read existing state
	value |= (1U<<7); //Set RST bit (bit 7)
	virtualWriteRegister(AS7265X_CONFIG, value); //Write value to config register to trigger soft reset
}


/* Obtaining various raw light readings */
//UV Readings
uint16_t getRawA()
{
	return (getChannel(AS7265X_RAW_R_G_A, AS72653_UV));
}
uint16_t getRawB()
{
	return (getChannel(AS7265X_RAW_S_H_B, AS72653_UV));
}
uint16_t getRawC()
{
	return (getChannel(AS7265X_RAW_T_I_C, AS72653_UV));
}
uint16_t getRawD()
{
	return (getChannel(AS7265X_RAW_U_J_D, AS72653_UV));
}
uint16_t getRawE()
{
	return (getChannel(AS7265X_RAW_V_K_E, AS72653_UV));
}
uint16_t getRawF()
{
	return (getChannel(AS7265X_RAW_W_L_F, AS72653_UV));
}

//VISIBLE Readings
uint16_t getRawG()
{
	return (getChannel(AS7265X_RAW_R_G_A, AS72652_VISIBLE));
}
uint16_t getRawH()
{
	return (getChannel(AS7265X_RAW_S_H_B, AS72652_VISIBLE));
}
uint16_t getRawI()
{
	return (getChannel(AS7265X_RAW_T_I_C, AS72652_VISIBLE));
}
uint16_t getRawJ()
{
	return (getChannel(AS7265X_RAW_U_J_D, AS72652_VISIBLE));
}
uint16_t getRawK()
{
	return (getChannel(AS7265X_RAW_V_K_E, AS72652_VISIBLE));
}
uint16_t getRawL()
{
	return (getChannel(AS7265X_RAW_W_L_F, AS72652_VISIBLE));
}

//NIR Readings
uint16_t getRawR()
{
	return (getChannel(AS7265X_RAW_R_G_A, AS72651_NIR));
}
uint16_t getRawS()
{
	return (getChannel(AS7265X_RAW_S_H_B, AS72651_NIR));
}
uint16_t getRawT()
{
	return (getChannel(AS7265X_RAW_T_I_C, AS72651_NIR));
}
uint16_t getRawU()
{
	return (getChannel(AS7265X_RAW_U_J_D, AS72651_NIR));
}
uint16_t getRawV()
{
	return (getChannel(AS7265X_RAW_V_K_E, AS72651_NIR));
}
uint16_t getRawW()
{
	return (getChannel(AS7265X_RAW_W_L_F, AS72651_NIR));
}

/* Obtaining the various calibrated light readings */
//UV Readings
float getCalibratedA()
{
	return (getCalibratedValue(AS7265X_CAL_R_G_A, AS72653_UV));
}
float getCalibratedB()
{
	return (getCalibratedValue(AS7265X_CAL_S_H_B, AS72653_UV));
}
float getCalibratedC()
{
	return (getCalibratedValue(AS7265X_CAL_T_I_C, AS72653_UV));
}
float getCalibratedD()
{
	return (getCalibratedValue(AS7265X_CAL_U_J_D, AS72653_UV));
}
float getCalibratedE()
{
	return (getCalibratedValue(AS7265X_CAL_V_K_E, AS72653_UV));
}
float getCalibratedF()
{
	return (getCalibratedValue(AS7265X_CAL_W_L_F, AS72653_UV));
}

//VISIBLE Readings
float getCalibratedG()
{
	return (getCalibratedValue(AS7265X_CAL_R_G_A, AS72652_VISIBLE));
}
float getCalibratedH()
{
	return (getCalibratedValue(AS7265X_CAL_S_H_B, AS72652_VISIBLE));
}
float getCalibratedI()
{
	return (getCalibratedValue(AS7265X_CAL_T_I_C, AS72652_VISIBLE));
}
float getCalibratedJ()
{
	return (getCalibratedValue(AS7265X_CAL_U_J_D, AS72652_VISIBLE));
}
float getCalibratedK()
{
	return (getCalibratedValue(AS7265X_CAL_V_K_E, AS72652_VISIBLE));
}
float getCalibratedL()
{
	return (getCalibratedValue(AS7265X_CAL_W_L_F, AS72652_VISIBLE));
}

//NIR Readings
float getCalibratedR()
{
	return (getCalibratedValue(AS7265X_CAL_R_G_A, AS72651_NIR));
}
float getCalibratedS()
{
	return (getCalibratedValue(AS7265X_CAL_S_H_B, AS72651_NIR));
}
float getCalibratedT()
{
	return (getCalibratedValue(AS7265X_CAL_T_I_C, AS72651_NIR));
}
float getCalibratedU()
{
	return (getCalibratedValue(AS7265X_CAL_U_J_D, AS72651_NIR));
}
float getCalibratedV()
{
	return (getCalibratedValue(AS7265X_CAL_V_K_E, AS72651_NIR));
}
float getCalibratedW()
{
	return (getCalibratedValue(AS7265X_CAL_W_L_F, AS72651_NIR));
}
