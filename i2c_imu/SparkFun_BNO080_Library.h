/*
 * SparkFun_BNO080_Library.h
 *
 *  Created on: Feb 6, 2019
 *      Author: josephmcgrath
 */

#ifndef SPARKFUN_BNO080_LIBRARY_H_
#define SPARKFUN_BNO080_LIBRARY_H_
#include <ti/drivers/I2C.h>



// IMU Functions
bool IMU_begin(I2C_Handle handle);
void IMU_softReset(I2C_Handle handle); //Try to reset the IMU via software

float qToFloat(int16_t fixedPointValue, uint8_t qPoint); //Given a Q value, converts fixed point floating to regular floating point number

bool IMU_receivePacket(I2C_Handle handle);
bool IMU_getData(I2C_Handle handle, uint16_t bytesRemaining); //Given a number of bytes, send the requests in I2C_BUFFER_LENGTH chunks
bool IMU_sendPacket(I2C_Handle handle, uint8_t channelNumber,
                    uint8_t dataLength);

void IMU_enableRotationVector(I2C_Handle handle, uint16_t timeBetweenReports);
void IMU_enableGyro(I2C_Handle handle, uint16_t timeBetweenReports);
void IMU_enableAccelerometer(I2C_Handle handle, uint16_t timeBetweenReports);

bool IMU_dataAvailable(I2C_Handle handle);
void parseInputReport(void); //Parse sensor readings out of report
void parseCommandReport(void); //Parse command responses out of report

float IMU_getQuatI();
float IMU_getQuatJ();
float IMU_getQuatK();
float IMU_getQuatReal();
float IMU_getQuatRadianAccuracy();
uint8_t IMU_getQuatAccuracy();

float IMU_getAccelX();
float IMU_getAccelY();
float IMU_getAccelZ();
uint8_t IMU_getAccelAccuracy();

float IMU_getGyroX();
float IMU_getGyroY();
float IMU_getGyroZ();
uint8_t IMU_getGyroAccuracy();

void setFeatureCommand(I2C_Handle handle, uint8_t reportID,
                       uint16_t timeBetweenReports);
void setFeatureCommand_config(I2C_Handle handle, uint8_t reportID,
                              uint16_t timeBetweenReports,
                              uint32_t specificConfig);



#endif /* SPARKFUN_BNO080_LIBRARY_H_ */
