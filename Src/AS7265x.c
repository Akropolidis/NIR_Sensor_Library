#include "AS7265X.h"


uint16_t maxWaitTime = 0; //Based on integration cycles

static uint16_t getChannel(uint8_t channelRegister, uint8_t device);
static float getCalibratedValue(uint8_t calAddress, uint8_t device);
static float convertBytesToFloat(uint32_t myLong);

static void selectDevice(uint8_t device); //Change between the x51, x52, or x53 for data and settings

static uint8_t virtualReadRegister(uint8_t virtualAddr);
static void virtualWriteRegister(uint8_t virtualAddr, uint8_t dataToWrite);

static uint8_t readRegister(uint8_t addr);
static void writeRegister(uint8_t addr, uint8_t val);


//Initializes the sensor with basic settings
//Returns false if sensor is not detected
bool begin()
{
	uart2_rxtx_init();
	I2C1_init();
}

//Reads from a given location from the AS726x
static uint8_t readRegister(uint8_t addr)
{
	uint8_t data = 0;
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
	while(1)
	{
		//Read slave I2C status to see if the read register is ready
		status = readRegister(AS7265X_STATUS_REG);
		if((status & AS7265X_TX_VALID) == 0) //New data may be written to WRITE register
		{
			break;
		}
		systickDelayMs(AS7265X_POLLING_DELAY); //Delay for 5 ms before checking for virtual register changes
	}

	//Send the virtual register address (disabling bit 7 to indicate a read).
	writeRegister(AS7265X_WRITE_REG, virtualAddr);

	while(1)
	{
		//Read slave I2C status to see if the read register is ready
		status = readRegister(AS7265X_STATUS_REG);
		if((status & AS7265X_RX_VALID) != 0) //New data may be written to WRITE register
		{
			break;
		}
		systickDelayMs(AS7265X_POLLING_DELAY); //Delay for 5 ms before checking for virtual register changes
	}

	data = readRegister(AS7265X_READ_REG);
	return data;
}

//Write to a virtual register in the AS7265x
static void virtualWriteRegister(uint8_t virtualAddr, uint8_t dataToWrite)
{
	volatile uint8_t status;

	//Wait for WRITE register to be empty
	while(1)
	{
		//Read slave I2C status to see if the write register is ready
		status = readRegister(AS7265X_STATUS_REG);
		if((status & AS7265X_TX_VALID) == 0) //New data may be written to WRITE register
		{
			break;
		}
		systickDelayMs(AS7265X_POLLING_DELAY); //Delay for 5 ms before checking for virtual register changes
	}

	//Send the virtual register address (enabling bit 7 to indicate a write).
	writeRegister(AS7265X_WRITE_REG, (virtualAddr | 1 << 7));

	//Wait for WRITE register to be empty
	while(1)
	{
		//Read slave I2C status to see if the write register is ready
		status = readRegister(AS7265X_STATUS_REG);
		if((status & AS7265X_TX_VALID) == 0) //New data may be written to WRITE register
		{
			break;
		}
		systickDelayMs(AS7265X_POLLING_DELAY); //Delay for 5 ms before checking for virtual register changes
	}

	//Send the data to complete the operation
	writeRegister(AS7265X_WRITE_REG, dataToWrite);
}

