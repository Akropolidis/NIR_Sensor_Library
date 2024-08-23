#ifndef AS7265X_H_
#define AS7265X_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "uart.h"
#include "i2c.h"
#include "systick.h"
#include "stm32f4xx.h"

#define SYS_CLK								16000000

/*I2C Slave Device Address and Physical Registers*/
#define AS7265X_ADDR						0x49
#define AS7265X_READ_ADDR					0x93
#define AS7265X_WRITE_ADDR					0x92

#define AS7265X_STATUS_REG					0x00
#define AS7265X_WRITE_REG					0x01
#define AS7265X_READ_REG					0x02

#define AS7265X_TX_VALID					0x02
#define AS7265X_RX_VALID					0x01


/*I2C Virtual Registers*/
#define AS7265X_HW_VERSION_HIGH				0x00
#define AS7265X_HW_VERSION_LOW				0x01

#define AS7265X_FW_VERSION_HIGH				0x02
#define AS7265X_FW_VERSION_LOW				0x03

#define AS7265X_CONFIG						0x04
#define AS7265X_INTEGRATION_TIME			0x05
#define AS7265X_DEVICE_TEMP					0x06
#define AS7265X_LED_CONFIG					0x07

//Raw Channel Registers
#define AS7265X_RAW_R_G_A					0x08
#define AS7265X_RAW_S_H_B					0x0A
#define AS7265X_RAW_T_I_C					0x0C
#define AS7265X_RAW_U_J_D					0x0E
#define AS7265X_RAW_V_K_E					0x10
#define AS7265X_RAW_W_L_F					0x12

//Calibrated Channel Registers
#define AS7265X_CAL_R_G_A 					0x14
#define AS7265X_CAL_S_H_B 					0x18
#define AS7265X_CAL_T_I_C 					0x1C
#define AS7265X_CAL_U_J_D 					0x20
#define AS7265X_CAL_V_K_E 					0x24
#define AS7265X_CAL_W_L_F					0x28

#define AS7265X_DEV_SELECT_CONTROL 			0x4F

#define AS7265X_COEF_DATA_0 				0x50
#define AS7265X_COEF_DATA_1 				0x51
#define AS7265X_COEF_DATA_2 				0x52
#define AS7265X_COEF_DATA_3 				0x53
#define AS7265X_COEF_DATA_READ 				0x54
#define AS7265X_COEF_DATA_WRITE 			0x55

//Settings

#define AS7265X_POLLING_DELAY 5 //Amount of ms to wait between checking for virtual register changes

#define AS72651_NIR 						0x00
#define AS72652_VISIBLE 					0x01
#define AS72653_UV 							0x02

#define AS7265x_LED_WHITE 					0x00 //White LED is connected to x51
#define AS7265x_LED_IR 						0x01 //IR LED is connected to x52
#define AS7265x_LED_UV 						0x02 //UV LED is connected to x53

#define AS7265X_LED_CURRENT_LIMIT_12_5MA 			0b00
#define AS7265X_LED_CURRENT_LIMIT_25MA 				0b01
#define AS7265X_LED_CURRENT_LIMIT_50MA 				0b10
#define AS7265X_LED_CURRENT_LIMIT_100MA 			0b11

#define AS7265X_INDICATOR_CURRENT_LIMIT_1MA 		0b00
#define AS7265X_INDICATOR_CURRENT_LIMIT_2MA 		0b01
#define AS7265X_INDICATOR_CURRENT_LIMIT_4MA 		0b10
#define AS7265X_INDICATOR_CURRENT_LIMIT_8MA 		0b11

#define AS7265X_GAIN_1X 							0b00
#define AS7265X_GAIN_37X 							0b01
#define AS7265X_GAIN_16X 							0b10
#define AS7265X_GAIN_64X 							0b11

#define AS7265X_MEASUREMENT_MODE_4CHAN 				0b00
#define AS7265X_MEASUREMENT_MODE_4CHAN_2 			0b01
#define AS7265X_MEASUREMENT_MODE_6CHAN_CONTINUOUS	0b10
#define AS7265X_MEASUREMENT_MODE_6CHAN_ONE_SHOT 	0b11

void begin();
bool isConnected(); //Checks if sensor ACK the I2C request
uint8_t getDeviceType();
uint8_t getHardwareVersion();
uint8_t getMajorFirmwareVersion();
uint8_t getPatchFirmwareVersion();
uint8_t getBuildFirmwareVersion();

uint8_t getTemperature(uint8_t deviceNumber); //Get temp in C of the master IC
float getTemperatureAverage();                    //Get average of all three ICs

void takeMeasurements();
void takeMeasurementsWithBulb();

void enableIndicator(); //Blue status LED
void disableIndicator();

void enableBulb(uint8_t device);
void disableBulb(uint8_t device);

void setGain(uint8_t gain);            //1 to 64x
void setMeasurementMode(uint8_t mode); //4 channel, other 4 channel, 6 chan, or 6 chan one shot
void setIntegrationCycles(uint8_t cycleValue);

void setBulbCurrent(uint8_t current, uint8_t device); //
void setIndicatorCurrent(uint8_t current);            //0 to 8mA

void enableInterrupt();
void disableInterrupt();

void softReset();

bool dataAvailable(); //Returns true when data is available

//Get the various raw readings
//UV - AS72653
uint16_t getRawA();
uint16_t getRawB();
uint16_t getRawC();
uint16_t getRawD();
uint16_t getRawE();
uint16_t getRawF();

//VISIBLE - AS72652
uint16_t getRawG();
uint16_t getRawH();
uint16_t getRawI();
uint16_t getRawJ();
uint16_t getRawK();
uint16_t getRawL();

//NIR - AS72651
uint16_t getRawR();
uint16_t getRawS();
uint16_t getRawT();
uint16_t getRawU();
uint16_t getRawV();
uint16_t getRawW();

//Returns the various calibration data
float getCalibratedA();
float getCalibratedB();
float getCalibratedC();
float getCalibratedD();
float getCalibratedE();
float getCalibratedF();

float getCalibratedG();
float getCalibratedH();
float getCalibratedI();
float getCalibratedJ();
float getCalibratedK();
float getCalibratedL();

float getCalibratedR();
float getCalibratedS();
float getCalibratedT();
float getCalibratedU();
float getCalibratedV();
float getCalibratedW();


#ifdef __cplusplus
}
#endif

#endif /* AS7265X_H_ */
